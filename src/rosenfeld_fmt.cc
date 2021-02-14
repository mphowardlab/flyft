#include "flyft/rosenfeld_fmt.h"

#include "flyft/reciprocal_mesh.h"

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
    allocate(state);
    auto mesh = state->getMesh();
    const ReciprocalMesh kmesh(*mesh);

    // compute n weights in fourier space
        {
        auto n0k = n0k_->data();
        auto n1k = n1k_->data();
        auto n2k = n2k_->data();
        auto n3k = n3k_->data();
        auto nv1k = nv1k_->data();
        auto nv2k = nv2k_->data();

        // zero the weights before accumulating by type
        std::fill(n0k, n0k+ft_->getReciprocalSize(), 0.);
        std::fill(n1k, n1k+ft_->getReciprocalSize(), 0.);
        std::fill(n2k, n2k+ft_->getReciprocalSize(), 0.);
        std::fill(n3k, n3k+ft_->getReciprocalSize(), 0.);
        std::fill(nv1k, nv1k+ft_->getReciprocalSize(), 0.);
        std::fill(nv2k, nv2k+ft_->getReciprocalSize(), 0.);

        for (const auto& t : state->getTypes())
            {
            // hard-sphere radius
            const double R = 0.5*state->getDiameter(t);
            if (R == 0.)
                {
                // no radius, no weights contribute (skip)
                continue;
                }

            // fft the density
            ft_->setRealData(state->getField(t)->data());
            ft_->transform();
            const auto rhok = ft_->getReciprocalData();

            // accumulate the fourier transformed densities into n
            for (int idx=0; idx < ft_->getReciprocalSize(); ++idx)
                {
                const double k = kmesh.coordinate(idx);

                // compute weights at this k, using limiting values for k = 0
                std::complex<double> w0,w1,w2,w3,wv1,wv2;
                computeWeights(w0,w1,w2,w3,wv1,wv2,k,R);

                n0k[idx] += w0*rhok[idx];
                n1k[idx] += w1*rhok[idx];
                n2k[idx] += w2*rhok[idx];
                n3k[idx] += w3*rhok[idx];
                nv1k[idx] += wv1*rhok[idx];
                nv2k[idx] += wv2*rhok[idx];
                }
            }
        }

    // transform n weights to real space to finish convolution
    // no need for a factor of mesh->step() here because w is analytical
        {
        ft_->setReciprocalData(n0k_->data());
        ft_->transform();
        std::copy(ft_->getRealData(),ft_->getRealData()+mesh->shape(),n0_->data());

        ft_->setReciprocalData(n1k_->data());
        ft_->transform();
        std::copy(ft_->getRealData(),ft_->getRealData()+mesh->shape(),n1_->data());

        ft_->setReciprocalData(n2k_->data());
        ft_->transform();
        std::copy(ft_->getRealData(),ft_->getRealData()+mesh->shape(),n2_->data());

        ft_->setReciprocalData(n3k_->data());
        ft_->transform();
        std::copy(ft_->getRealData(),ft_->getRealData()+mesh->shape(),n3_->data());

        ft_->setReciprocalData(nv1k_->data());
        ft_->transform();
        std::copy(ft_->getRealData(),ft_->getRealData()+mesh->shape(),nv1_->data());

        ft_->setReciprocalData(nv2k_->data());
        ft_->transform();
        std::copy(ft_->getRealData(),ft_->getRealData()+mesh->shape(),nv2_->data());
        }

    // evaluate phi and partial derivatives in real space using n
        {
        auto n0 = n0_->data();
        auto n1 = n1_->data();
        auto n2 = n2_->data();
        auto n3 = n3_->data();
        auto nv1 = nv1_->data();
        auto nv2 = nv2_->data();

        auto phi = phi_->data();
        auto dphi_dn0 = dphi_dn0_->data();
        auto dphi_dn1 = dphi_dn1_->data();
        auto dphi_dn2 = dphi_dn2_->data();
        auto dphi_dn3 = dphi_dn3_->data();
        auto dphi_dnv1 = dphi_dnv1_->data();
        auto dphi_dnv2 = dphi_dnv2_->data();

        //////////////// The functions in here could be templated out too
        for (int idx=0; idx < mesh->shape(); ++idx)
            {
            // precompute the "void fraction" vf, which is only a function of n3
            const double vf = 1.-n3[idx];
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

            phi[idx] = (f1*n0[idx]
                        +f2*(n1[idx]*n2[idx]-nv1[idx]*nv2[idx])
                        +f4*(n2[idx]*n2[idx]*n2[idx]-3.*n2[idx]*nv2[idx]*nv2[idx]));

            dphi_dn0[idx] = f1;
            dphi_dn1[idx] = f2*n2[idx];
            //////// these can be optimized to reuse quantities from above
            dphi_dn2[idx] = f2*n1[idx] + 3.*f4*(n2[idx]*n2[idx]-nv2[idx]*nv2[idx]);
            dphi_dn3[idx] = (df1*n0[idx]
                             +df2*(n1[idx]*n2[idx]-nv1[idx]*nv2[idx])
                             +df4*(n2[idx]*n2[idx]*n2[idx]-3.*n2[idx]*nv2[idx]*nv2[idx]));
            ////////
            dphi_dnv1[idx] = -f2*nv2[idx];
            dphi_dnv2[idx] = -f2*nv1[idx]-6.*f4*n2[idx]*nv2[idx];
            }
        ////////////////
        }

    // convert phi derivatives to Fourier space
        {
        ft_->setRealData(dphi_dn0_->data());
        ft_->transform();
        std::copy(ft_->getReciprocalData(),ft_->getReciprocalData()+ft_->getReciprocalSize(),dphi_dn0k_->data());

        ft_->setRealData(dphi_dn1_->data());
        ft_->transform();
        std::copy(ft_->getReciprocalData(),ft_->getReciprocalData()+ft_->getReciprocalSize(),dphi_dn1k_->data());

        ft_->setRealData(dphi_dn2_->data());
        ft_->transform();
        std::copy(ft_->getReciprocalData(),ft_->getReciprocalData()+ft_->getReciprocalSize(),dphi_dn2k_->data());

        ft_->setRealData(dphi_dn3_->data());
        ft_->transform();
        std::copy(ft_->getReciprocalData(),ft_->getReciprocalData()+ft_->getReciprocalSize(),dphi_dn3k_->data());

        ft_->setRealData(dphi_dnv1_->data());
        ft_->transform();
        std::copy(ft_->getReciprocalData(),ft_->getReciprocalData()+ft_->getReciprocalSize(),dphi_dnv1k_->data());

        ft_->setRealData(dphi_dnv2_->data());
        ft_->transform();
        std::copy(ft_->getReciprocalData(),ft_->getReciprocalData()+ft_->getReciprocalSize(),dphi_dnv2k_->data());
        }

    // convolve phi derivatives with weights to get functional derivatives
    // again, no need for a factor of mesh->step() here because w is analytical
        {
        auto dphi_dn0k = dphi_dn0k_->data();
        auto dphi_dn1k = dphi_dn1k_->data();
        auto dphi_dn2k = dphi_dn2k_->data();
        auto dphi_dn3k = dphi_dn3k_->data();
        auto dphi_dnv1k = dphi_dnv1k_->data();
        auto dphi_dnv2k = dphi_dnv2k_->data();

        auto derivativek = derivativek_->data();

        for (const auto& t : state->getTypes())
            {
            // hard-sphere radius
            const double R = 0.5*state->getDiameter(t);
            if (R == 0.)
                {
                // no radius, no contribution to energy
                // need to set here as we are not prefilling the array with zeros
                std::fill(derivatives_[t]->data(), derivatives_[t]->data()+mesh->shape(),0.0);
                continue;
                }

            for (int idx=0; idx < ft_->getReciprocalSize(); ++idx)
                {
                const double k = kmesh.coordinate(idx);

                // get weights
                std::complex<double> w0,w1,w2,w3,wv1,wv2;
                computeWeights(w0,w1,w2,w3,wv1,wv2,k,R);

                // convolution (note opposite sign for vector weights due to change of order in convolution)
                derivativek[idx] = (dphi_dn0k[idx]*w0+dphi_dn1k[idx]*w1+dphi_dn2k[idx]*w2+dphi_dn3k[idx]*w3
                                    -dphi_dnv1k[idx]*wv1-dphi_dnv2k[idx]*wv2);
                }
            ft_->setReciprocalData(derivativek);
            ft_->transform();
            std::copy(ft_->getRealData(),ft_->getRealData()+mesh->shape(),derivatives_[t]->data());
            }
        }

    // total value of free energy is integral of phi, which we do by simple quadrature
    value_ = mesh->step()*std::accumulate(phi_->data(), phi_->data()+mesh->shape(), 0.0);
    }

void RosenfeldFMT::allocate(std::shared_ptr<State> state)
    {
    FreeEnergyFunctional::allocate(state);

    auto mesh = state->getMesh();

    // update Fourier transform to mesh shape
    if (!ft_ || (ft_->getRealSize() != mesh->shape()))
        {
        ft_ = std::make_unique<FourierTransform>(mesh->shape());
        }

    // update shape of internal fields (alloc handled in a smart way by Field)
    setupFourierFields(n0_, n0k_, mesh->shape(), ft_->getReciprocalSize());
    setupFourierFields(n1_, n1k_, mesh->shape(), ft_->getReciprocalSize());
    setupFourierFields(n2_, n2k_, mesh->shape(), ft_->getReciprocalSize());
    setupFourierFields(n3_, n3k_, mesh->shape(), ft_->getReciprocalSize());
    setupFourierFields(nv1_, nv1k_, mesh->shape(), ft_->getReciprocalSize());
    setupFourierFields(nv2_, nv2k_, mesh->shape(), ft_->getReciprocalSize());

    setupFourierFields(dphi_dn0_, dphi_dn0k_, mesh->shape(), ft_->getReciprocalSize());
    setupFourierFields(dphi_dn1_, dphi_dn1k_, mesh->shape(), ft_->getReciprocalSize());
    setupFourierFields(dphi_dn2_, dphi_dn2k_, mesh->shape(), ft_->getReciprocalSize());
    setupFourierFields(dphi_dn3_, dphi_dn3k_, mesh->shape(), ft_->getReciprocalSize());
    setupFourierFields(dphi_dnv1_, dphi_dnv1k_, mesh->shape(), ft_->getReciprocalSize());
    setupFourierFields(dphi_dnv2_, dphi_dnv2k_, mesh->shape(), ft_->getReciprocalSize());

    // these two fields only exist in one space
    setupField(phi_,mesh->shape());
    setupComplexField(derivativek_, ft_->getReciprocalSize());
    }

void RosenfeldFMT::setupField(std::unique_ptr<Field>& field, int shape)
    {
    if (!field)
        {
        field = std::make_unique<Field>(shape);
        }
    field->reshape(shape);
    }

void RosenfeldFMT::setupComplexField(std::unique_ptr<ComplexField>& kfield, int shape)
    {
    if (!kfield)
        {
        kfield = std::make_unique<ComplexField>(shape);
        }
    kfield->reshape(shape);
    }

void RosenfeldFMT::setupFourierFields(std::unique_ptr<Field>& field,
                                      std::unique_ptr<ComplexField>& kfield,
                                      int shape,
                                      int kshape)
    {
    setupField(field,shape);
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

}
