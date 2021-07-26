#include "flyft/reciprocal_mesh.h"
#include "flyft/rosenfeld_fmt.h"

#include <algorithm>
#include <cmath>
#include <complex>
#include <numeric>

namespace flyft
{

void RosenfeldFMT::compute(std::shared_ptr<State> state)
    {
    // (re-)allocate the memory needed to work with this state
    // kmesh should really be coming from somewhere else (like the FFT)
    auto max_d = 0.0;
    for (const auto& t : state->getTypes())
        {
        max_d = std::max(max_d,diameters_.at(t));
        }
    state->requestBuffer(0.5*max_d);

    // TODO: revamp ReciprocalMesh for MPI, this will still work for now
    const auto mesh = *state->getMesh();
    const ReciprocalMesh kmesh(mesh.step()*mesh.shape(),mesh.shape());
    allocate(state);

    // compute n weights in fourier space
        {
        // zero the weights before accumulating by type
        std::fill(n0k_->begin(), n0k_->end(), 0.);
        std::fill(n1k_->begin(), n1k_->end(), 0.);
        std::fill(n2k_->begin(), n2k_->end(), 0.);
        std::fill(n3k_->begin(), n3k_->end(), 0.);
        std::fill(nv1k_->begin(), nv1k_->end(), 0.);
        std::fill(nv2k_->begin(), nv2k_->end(), 0.);

        for (const auto& t : state->getTypes())
            {
            // hard-sphere radius
            const double R = 0.5*diameters_.at(t);
            if (R == 0.)
                {
                // no radius, no weights contribute (skip)
                continue;
                }

            // fft the density
            ft_->setRealData(state->getField(t)->begin().get());
            ft_->transform();
            auto rhok = ft_->getReciprocalData();

            // accumulate the fourier transformed densities into n
            auto n0k = n0k_->begin();
            auto n1k = n1k_->begin();
            auto n2k = n2k_->begin();
            auto n3k = n3k_->begin();
            auto nv1k = nv1k_->begin();
            auto nv2k = nv2k_->begin();
            const auto size = ft_->getReciprocalSize();
            #ifdef FLYFT_OPENMP
            #pragma omp parallel for schedule(static) default(none) firstprivate(R,size,kmesh) shared(rhok,n0k,n1k,n2k,n3k,nv1k,nv2k)
            #endif
            for (int idx=0; idx < size; ++idx)
                {
                const double k = kmesh.coordinate(idx);

                // compute weights at this k, using limiting values for k = 0
                std::complex<double> w0,w1,w2,w3,wv1,wv2;
                computeWeights(w0,w1,w2,w3,wv1,wv2,k,R);

                n0k(idx) += w0*rhok[idx];
                n1k(idx) += w1*rhok[idx];
                n2k(idx) += w2*rhok[idx];
                n3k(idx) += w3*rhok[idx];
                nv1k(idx) += wv1*rhok[idx];
                nv2k(idx) += wv2*rhok[idx];
                }
            }
        }

    // transform n weights to real space to finish convolution
    // no need for a factor of mesh.step() here because w is analytical
        {
        ft_->setReciprocalData(n0k_->cbegin().get());
        ft_->transform();
        std::copy(ft_->getRealData(),ft_->getRealData()+ft_->getRealSize(),n0_->begin().get());

        ft_->setReciprocalData(n1k_->cbegin().get());
        ft_->transform();
        std::copy(ft_->getRealData(),ft_->getRealData()+ft_->getRealSize(),n1_->begin().get());

        ft_->setReciprocalData(n2k_->cbegin().get());
        ft_->transform();
        std::copy(ft_->getRealData(),ft_->getRealData()+ft_->getRealSize(),n2_->begin().get());

        ft_->setReciprocalData(n3k_->cbegin().get());
        ft_->transform();
        std::copy(ft_->getRealData(),ft_->getRealData()+ft_->getRealSize(),n3_->begin().get());

        ft_->setReciprocalData(nv1k_->cbegin().get());
        ft_->transform();
        std::copy(ft_->getRealData(),ft_->getRealData()+ft_->getRealSize(),nv1_->begin().get());

        ft_->setReciprocalData(nv2k_->cbegin().get());
        ft_->transform();
        std::copy(ft_->getRealData(),ft_->getRealData()+ft_->getRealSize(),nv2_->begin().get());
        }

    // evaluate phi and partial derivatives in real space using n
        {
        auto n0 = n0_->cbegin();
        auto n1 = n1_->cbegin();
        auto n2 = n2_->cbegin();
        auto n3 = n3_->cbegin();
        auto nv1 = nv1_->cbegin();
        auto nv2 = nv2_->cbegin();

        auto phi = phi_->begin();
        auto dphi_dn0 = dphi_dn0_->begin();
        auto dphi_dn1 = dphi_dn1_->begin();
        auto dphi_dn2 = dphi_dn2_->begin();
        auto dphi_dn3 = dphi_dn3_->begin();
        auto dphi_dnv1 = dphi_dnv1_->begin();
        auto dphi_dnv2 = dphi_dnv2_->begin();

        //////////////// The functions in here could be templated out too
        const auto size = ft_->getRealSize();
        #ifdef FLYFT_OPENMP
        #pragma omp parallel for schedule(static) default(none) firstprivate(size) shared(n0,n1,n2,n3,nv1,nv2,phi,dphi_dn0,dphi_dn1,dphi_dn2,dphi_dn3,dphi_dnv1,dphi_dnv2)
        #endif
        for (int idx=0; idx < size; ++idx)
            {
            // precompute the "void fraction" vf, which is only a function of n3
            const double vf = 1.-n3(idx);
            if (vf < 0)
                {
                // local void fraction unphysical
                }
            const double logvf = std::log(vf);
            const double vfinv = 1./vf;

            // these are all functions of n3 (via vf)
            const double f1 = -logvf;
            const double df1 = vfinv;
            const double f2 = vfinv;
            const double df2 = vfinv*vfinv;
            const double f4 = vfinv*vfinv/(24.*M_PI);
            const double df4 = 2.*f4*vfinv;

            phi(idx) = (f1*n0(idx)
                        +f2*(n1(idx)*n2(idx)-nv1(idx)*nv2(idx))
                        +f4*(n2(idx)*n2(idx)*n2(idx)-3.*n2(idx)*nv2(idx)*nv2(idx)));

            dphi_dn0(idx) = f1;
            dphi_dn1(idx) = f2*n2(idx);
            //////// these can be optimized to reuse quantities from above
            dphi_dn2(idx) = f2*n1(idx) + 3.*f4*(n2(idx)*n2(idx)-nv2(idx)*nv2(idx));
            dphi_dn3(idx) = (df1*n0(idx)
                             +df2*(n1(idx)*n2(idx)-nv1(idx)*nv2(idx))
                             +df4*(n2(idx)*n2(idx)*n2(idx)-3.*n2(idx)*nv2(idx)*nv2(idx)));
            ////////
            dphi_dnv1(idx) = -f2*nv2(idx);
            dphi_dnv2(idx) = -f2*nv1(idx)-6.*f4*n2(idx)*nv2(idx);
            }
        ////////////////
        }

    // convert phi derivatives to Fourier space
        {
        ft_->setRealData(dphi_dn0_->cbegin().get());
        ft_->transform();
        std::copy(ft_->getReciprocalData(),ft_->getReciprocalData()+ft_->getReciprocalSize(),dphi_dn0k_->begin().get());

        ft_->setRealData(dphi_dn1_->cbegin().get());
        ft_->transform();
        std::copy(ft_->getReciprocalData(),ft_->getReciprocalData()+ft_->getReciprocalSize(),dphi_dn1k_->begin().get());

        ft_->setRealData(dphi_dn2_->cbegin().get());
        ft_->transform();
        std::copy(ft_->getReciprocalData(),ft_->getReciprocalData()+ft_->getReciprocalSize(),dphi_dn2k_->begin().get());

        ft_->setRealData(dphi_dn3_->cbegin().get());
        ft_->transform();
        std::copy(ft_->getReciprocalData(),ft_->getReciprocalData()+ft_->getReciprocalSize(),dphi_dn3k_->begin().get());

        ft_->setRealData(dphi_dnv1_->cbegin().get());
        ft_->transform();
        std::copy(ft_->getReciprocalData(),ft_->getReciprocalData()+ft_->getReciprocalSize(),dphi_dnv1k_->begin().get());

        ft_->setRealData(dphi_dnv2_->cbegin().get());
        ft_->transform();
        std::copy(ft_->getReciprocalData(),ft_->getReciprocalData()+ft_->getReciprocalSize(),dphi_dnv2k_->begin().get());
        }

    // convolve phi derivatives with weights to get functional derivatives
    // again, no need for a factor of mesh.step() here because w is analytical
        {
        auto dphi_dn0k = dphi_dn0k_->cbegin();
        auto dphi_dn1k = dphi_dn1k_->cbegin();
        auto dphi_dn2k = dphi_dn2k_->cbegin();
        auto dphi_dn3k = dphi_dn3k_->cbegin();
        auto dphi_dnv1k = dphi_dnv1k_->cbegin();
        auto dphi_dnv2k = dphi_dnv2k_->cbegin();

        auto derivativek = derivativek_->begin();
        for (const auto& t : state->getTypes())
            {
            // hard-sphere radius
            const double R = 0.5*diameters_.at(t);
            if (R == 0.)
                {
                // no radius, no contribution to energy
                // need to set here as we are not prefilling the array with zeros
                auto derivative = derivatives_.at(t);
                std::fill(derivative->begin(), derivative->end(),0.0);
                continue;
                }

            const auto size = ft_->getReciprocalSize();
            #ifdef FLYFT_OPENMP
            #pragma omp parallel for schedule(static) default(none) firstprivate(size,kmesh,R) \
            shared(derivativek,dphi_dn0k,dphi_dn1k,dphi_dn2k,dphi_dn3k,dphi_dnv1k,dphi_dnv2k)
            #endif
            for (int idx=0; idx < size; ++idx)
                {
                const double k = kmesh.coordinate(idx);

                // get weights
                std::complex<double> w0,w1,w2,w3,wv1,wv2;
                computeWeights(w0,w1,w2,w3,wv1,wv2,k,R);

                // convolution (note opposite sign for vector weights due to change of order in convolution)
                derivativek(idx) = (dphi_dn0k(idx)*w0+dphi_dn1k(idx)*w1+dphi_dn2k(idx)*w2+dphi_dn3k(idx)*w3
                                    -dphi_dnv1k(idx)*wv1-dphi_dnv2k(idx)*wv2);
                }
            ft_->setReciprocalData(derivativek.get());
            ft_->transform();

            // copy the valid values
            auto din = ft_->getRealData();
            auto dout = derivatives_.at(t)->begin();
            #ifdef FLYFT_OPENMP
            #pragma omp parallel for schedule(static) default(none) firstprivate(mesh) shared(din,dout)
            #endif
            for (int idx=0; idx < mesh.shape(); ++idx)
                {
                dout(idx) = din[idx];
                }
            }
        }

    // accumulate value
        {
        auto phi = phi_->cbegin();
        value_ = 0.0;
        #ifdef FLYFT_OPENMP
        #pragma omp parallel for schedule(static) default(none) firstprivate(mesh) shared(phi) reduction(+:value_)
        #endif
        for (int idx=0; idx < mesh.shape(); ++idx)
            {
            value_ += mesh.step()*phi(idx);
            }
        }
    }

void RosenfeldFMT::allocate(std::shared_ptr<State> state)
    {
    Functional::allocate(state);

    const auto mesh = *state->getMesh();

    // update Fourier transform to mesh shape
    if (!ft_ || (ft_->getRealSize() != mesh.shape()))
        {
        ft_ = std::make_unique<FourierTransform>(mesh.shape());
        }

    // update shape of internal fields (alloc handled in a smart way by Field)
    // TODO: use plain old data for this stuff, since it does not need advanced padding?
    setupFourierFields(n0_, n0k_, mesh, ft_->getReciprocalSize());
    setupFourierFields(n1_, n1k_, mesh, ft_->getReciprocalSize());
    setupFourierFields(n2_, n2k_, mesh, ft_->getReciprocalSize());
    setupFourierFields(n3_, n3k_, mesh, ft_->getReciprocalSize());
    setupFourierFields(nv1_, nv1k_, mesh, ft_->getReciprocalSize());
    setupFourierFields(nv2_, nv2k_, mesh, ft_->getReciprocalSize());

    setupFourierFields(dphi_dn0_, dphi_dn0k_, mesh, ft_->getReciprocalSize());
    setupFourierFields(dphi_dn1_, dphi_dn1k_, mesh, ft_->getReciprocalSize());
    setupFourierFields(dphi_dn2_, dphi_dn2k_, mesh, ft_->getReciprocalSize());
    setupFourierFields(dphi_dn3_, dphi_dn3k_, mesh, ft_->getReciprocalSize());
    setupFourierFields(dphi_dnv1_, dphi_dnv1k_, mesh, ft_->getReciprocalSize());
    setupFourierFields(dphi_dnv2_, dphi_dnv2k_, mesh, ft_->getReciprocalSize());

    // these two fields only exist in one space
    setupField(phi_,mesh);
    setupComplexField(derivativek_, ft_->getReciprocalSize());
    }

void RosenfeldFMT::setupField(std::unique_ptr<Field>& field, const Mesh& mesh)
    {
    if (!field)
        {
        field = std::make_unique<Field>(mesh.shape(),0);
        }
    else
        {
        field->reshape(mesh.shape(),0);
        }
    }

void RosenfeldFMT::setupComplexField(std::unique_ptr<ComplexField>& kfield, int shape)
    {
    if (!kfield)
        {
        kfield = std::make_unique<ComplexField>(shape,0);
        }
    else
        {
        kfield->reshape(shape,0);
        }
    }

void RosenfeldFMT::setupFourierFields(std::unique_ptr<Field>& field,
                                      std::unique_ptr<ComplexField>& kfield,
                                      const Mesh& mesh,
                                      int kshape)
    {
    setupField(field,mesh);
    setupComplexField(kfield,kshape);
    }

void RosenfeldFMT::computeWeights(std::complex<double>& w0,
                                  std::complex<double>& w1,
                                  std::complex<double>& w2,
                                  std::complex<double>& w3,
                                  std::complex<double>& wv1,
                                  std::complex<double>& wv2,
                                  double k,
                                  double R) const

    {
    if (k != 0.)
        {
        const double kR = k*R;
        const double sinkR = std::sin(kR);
        const double coskR = std::cos(kR);

        w0 = sinkR/kR;
        w1 = R*w0;
        w2 = (4.*M_PI)*R*w1;
        w3 = (4.*M_PI)*(sinkR-kR*coskR)/(k*k*k);
        // do these in opposite order because wv2 follows easily from w3
        wv2 = std::complex<double>(0.,-k)*w3;
        wv1 = wv2/(4.*M_PI*R);
        }
    else
        {
        w0 = 1.0;
        w1 = R;
        w2 = (4.*M_PI)*R*w1;
        w3 = w2*(R/3.);
        wv1 = 0.0;
        wv2 = 0.0;
        }
    }

const TypeMap<double>& RosenfeldFMT::getDiameters()
    {
    return diameters_;
    }

double RosenfeldFMT::getDiameter(const std::string& type) const
    {
    return diameters_.at(type);
    }

void RosenfeldFMT::setDiameters(const TypeMap<double>& diameters)
    {
    diameters_ = TypeMap<double>(diameters);
    }

void RosenfeldFMT::setDiameter(const std::string& type, double diameter)
    {
    if (diameter >= 0.)
        {
        diameters_[type] = diameter;
        }
    else
        {
        // error: invalid diameter
        }
    }

}
