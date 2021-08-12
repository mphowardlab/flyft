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
    // TODO: revamp ReciprocalMesh for MPI, this will still work for now
    setup(state);
    const auto mesh = *state->getMesh()->local();
    const ReciprocalMesh kmesh(mesh.asLength(layout_.shape()),layout_.shape());

    // compute n weights in fourier space
        {
        // zero the weights before accumulating by type
        std::fill(n0k_->full_view().begin(), n0k_->full_view().end(), 0.);
        std::fill(n1k_->full_view().begin(), n1k_->full_view().end(), 0.);
        std::fill(n2k_->full_view().begin(), n2k_->full_view().end(), 0.);
        std::fill(n3k_->full_view().begin(), n3k_->full_view().end(), 0.);
        std::fill(nv1k_->full_view().begin(), nv1k_->full_view().end(), 0.);
        std::fill(nv2k_->full_view().begin(), nv2k_->full_view().end(), 0.);

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
            ft_->setRealData(state->getField(t)->full_view().begin().get());
            ft_->transform();
            auto rhok = ft_->getReciprocalData();

            // accumulate the fourier transformed densities into n
            auto n0k = n0k_->full_view();
            auto n1k = n1k_->full_view();
            auto n2k = n2k_->full_view();
            auto n3k = n3k_->full_view();
            auto nv1k = nv1k_->full_view();
            auto nv2k = nv2k_->full_view();
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
        ft_->setReciprocalData(n0k_->const_full_view().begin().get());
        ft_->transform();
        std::copy(ft_->getRealData(),ft_->getRealData()+ft_->getRealSize(),n0_->full_view().begin().get());

        ft_->setReciprocalData(n1k_->const_full_view().begin().get());
        ft_->transform();
        std::copy(ft_->getRealData(),ft_->getRealData()+ft_->getRealSize(),n1_->full_view().begin().get());

        ft_->setReciprocalData(n2k_->const_full_view().begin().get());
        ft_->transform();
        std::copy(ft_->getRealData(),ft_->getRealData()+ft_->getRealSize(),n2_->full_view().begin().get());

        ft_->setReciprocalData(n3k_->const_full_view().begin().get());
        ft_->transform();
        std::copy(ft_->getRealData(),ft_->getRealData()+ft_->getRealSize(),n3_->full_view().begin().get());

        ft_->setReciprocalData(nv1k_->const_full_view().begin().get());
        ft_->transform();
        std::copy(ft_->getRealData(),ft_->getRealData()+ft_->getRealSize(),nv1_->full_view().begin().get());

        ft_->setReciprocalData(nv2k_->const_full_view().begin().get());
        ft_->transform();
        std::copy(ft_->getRealData(),ft_->getRealData()+ft_->getRealSize(),nv2_->full_view().begin().get());
        }

    // evaluate phi and partial derivatives in real space using n
        {
        auto n0 = n0_->const_view();
        auto n1 = n1_->const_view();
        auto n2 = n2_->const_view();
        auto n3 = n3_->const_view();
        auto nv1 = nv1_->const_view();
        auto nv2 = nv2_->const_view();

        auto phi = phi_->view();
        auto dphi_dn0 = dphi_dn0_->view();
        auto dphi_dn1 = dphi_dn1_->view();
        auto dphi_dn2 = dphi_dn2_->view();
        auto dphi_dn3 = dphi_dn3_->view();
        auto dphi_dnv1 = dphi_dnv1_->view();
        auto dphi_dnv2 = dphi_dnv2_->view();

        //////////////// The functions in here could be templated out too
        #ifdef FLYFT_OPENMP
        #pragma omp parallel for schedule(static) default(none) firstprivate(mesh) shared(n0,n1,n2,n3,nv1,nv2,phi,dphi_dn0,dphi_dn1,dphi_dn2,dphi_dn3,dphi_dnv1,dphi_dnv2)
        #endif
        for (int idx=0; idx < mesh.shape(); ++idx)
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

    // sync buffers for second fourier transform
        {
        auto comm = state->getMesh();
        comm->sync(dphi_dn0_);
        comm->sync(dphi_dn1_);
        comm->sync(dphi_dn2_);
        comm->sync(dphi_dn3_);
        comm->sync(dphi_dnv1_);
        comm->sync(dphi_dnv2_);
        }

    // convert phi derivatives to Fourier space
        {
        ft_->setRealData(dphi_dn0_->const_full_view().begin().get());
        ft_->transform();
        std::copy(ft_->getReciprocalData(),ft_->getReciprocalData()+ft_->getReciprocalSize(),dphi_dn0k_->full_view().begin().get());

        ft_->setRealData(dphi_dn1_->const_full_view().begin().get());
        ft_->transform();
        std::copy(ft_->getReciprocalData(),ft_->getReciprocalData()+ft_->getReciprocalSize(),dphi_dn1k_->full_view().begin().get());

        ft_->setRealData(dphi_dn2_->const_full_view().begin().get());
        ft_->transform();
        std::copy(ft_->getReciprocalData(),ft_->getReciprocalData()+ft_->getReciprocalSize(),dphi_dn2k_->full_view().begin().get());

        ft_->setRealData(dphi_dn3_->const_full_view().begin().get());
        ft_->transform();
        std::copy(ft_->getReciprocalData(),ft_->getReciprocalData()+ft_->getReciprocalSize(),dphi_dn3k_->full_view().begin().get());

        ft_->setRealData(dphi_dnv1_->const_full_view().begin().get());
        ft_->transform();
        std::copy(ft_->getReciprocalData(),ft_->getReciprocalData()+ft_->getReciprocalSize(),dphi_dnv1k_->full_view().begin().get());

        ft_->setRealData(dphi_dnv2_->const_full_view().begin().get());
        ft_->transform();
        std::copy(ft_->getReciprocalData(),ft_->getReciprocalData()+ft_->getReciprocalSize(),dphi_dnv2k_->full_view().begin().get());
        }

    // convolve phi derivatives with weights to get functional derivatives
    // again, no need for a factor of mesh.step() here because w is analytical
        {
        auto dphi_dn0k = dphi_dn0k_->const_full_view();
        auto dphi_dn1k = dphi_dn1k_->const_full_view();
        auto dphi_dn2k = dphi_dn2k_->const_full_view();
        auto dphi_dn3k = dphi_dn3k_->const_full_view();
        auto dphi_dnv1k = dphi_dnv1k_->const_full_view();
        auto dphi_dnv2k = dphi_dnv2k_->const_full_view();

        auto derivativek = derivativek_->full_view();
        for (const auto& t : state->getTypes())
            {
            // hard-sphere radius
            const double R = 0.5*diameters_.at(t);
            if (R == 0.)
                {
                // no radius, no contribution to energy
                // need to set here as we are not prefilling the array with zeros
                auto derivative = derivatives_.at(t)->view();
                std::fill(derivative.begin(), derivative.end(),0.0);
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
            ft_->setReciprocalData(derivativek.begin().get());
            ft_->transform();

            // copy the valid values
            Field::ConstantView din(ft_->getRealData(),layout_,buffer_shape_,buffer_shape_+mesh.shape());
            auto dout = derivatives_.at(t)->view();
            #ifdef FLYFT_OPENMP
            #pragma omp parallel for schedule(static) default(none) firstprivate(mesh) shared(din,dout)
            #endif
            for (int idx=0; idx < mesh.shape(); ++idx)
                {
                dout(idx) = din(idx);
                }
            }
        }

    // accumulate value
        {
        auto phi = phi_->const_view();
        value_ = 0.0;
        #ifdef FLYFT_OPENMP
        #pragma omp parallel for schedule(static) default(none) firstprivate(mesh) shared(phi) reduction(+:value_)
        #endif
        for (int idx=0; idx < mesh.shape(); ++idx)
            {
            value_ += mesh.step()*phi(idx);
            }
        }

    value_ = state->getCommunicator()->sum(value_);
    }

int RosenfeldFMT::determineBufferShape(std::shared_ptr<State> state, const std::string& /*type*/)
    {
    // TODO: cache this
    const double buffer = std::max_element(diameters_.begin(),diameters_.end())->second;
    buffer_shape_ = state->getMesh()->local()->asShape(buffer);
    return buffer_shape_;
    }

void RosenfeldFMT::setup(std::shared_ptr<State> state)
    {
    Functional::setup(state);

    // update Fourier transform to mesh shape + buffer
    const auto mesh = *state->getMesh()->local();
    layout_ = DataLayout(mesh.shape()+2*buffer_shape_);
    if (!ft_ || (ft_->getRealSize() != layout_.shape()))
        {
        ft_ = std::make_unique<FourierTransform>(layout_.shape());
        }

    // update shape of internal fields (alloc handled in a smart way by Field)
    // TODO: use plain old data for this stuff, since it does not need advanced padding?
    setupField(n0_); setupComplexField(n0k_); setupField(dphi_dn0_); setupComplexField(dphi_dn0k_);
    setupField(n1_); setupComplexField(n1k_); setupField(dphi_dn1_); setupComplexField(dphi_dn1k_);
    setupField(n2_); setupComplexField(n2k_); setupField(dphi_dn2_); setupComplexField(dphi_dn2k_);
    setupField(n3_); setupComplexField(n3k_); setupField(dphi_dn3_); setupComplexField(dphi_dn3k_);
    setupField(nv1_); setupComplexField(nv1k_); setupField(dphi_dnv1_); setupComplexField(dphi_dnv1k_);
    setupField(nv2_); setupComplexField(nv2k_); setupField(dphi_dnv2_); setupComplexField(dphi_dnv2k_);

    // these two fields only exist in one space
    setupField(phi_);
    setupComplexField(derivativek_);
    }

void RosenfeldFMT::setupField(std::shared_ptr<Field>& field)
    {
    if (!field)
        {
        field = std::make_shared<Field>(ft_->getRealSize()-2*buffer_shape_,buffer_shape_);
        }
    else
        {
        field->reshape(ft_->getRealSize()-2*buffer_shape_,buffer_shape_);
        }
    }

void RosenfeldFMT::setupComplexField(std::unique_ptr<ComplexField>& kfield)
    {
    if (!kfield)
        {
        kfield = std::make_unique<ComplexField>(ft_->getReciprocalSize(),0);
        }
    else
        {
        kfield->reshape(ft_->getReciprocalSize(),0);
        }
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
