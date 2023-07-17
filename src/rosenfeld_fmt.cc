#include "flyft/rosenfeld_fmt.h"

#include "flyft/cartesian_mesh.h"
#include "flyft/spherical_mesh.h"

#include <algorithm>
#include <cmath>
#include <complex>
#include <numeric>

namespace flyft
{

RosenfeldFMT::RosenfeldFMT()
    {
    compute_depends_.add(&diameters_);
    }

void RosenfeldFMT::_compute(std::shared_ptr<State> state, bool compute_value)
    {
    // ensure fields are sync'd before starting
    state->syncFields();

    const auto mesh = state->getMesh()->local().get();
    const auto conv_type = getConvolutionType(state->getMesh()->local());

    // temporary storage for spherical convolutions
    if (conv_type == ConvolutionType::spherical)
        {
        setupField(tmp_r_field_);
        }

    // compute weighted densities
    if (conv_type == ConvolutionType::cartesian)
        {
        computeCartesianWeightedDensities(state);
        }
    else if (conv_type == ConvolutionType::spherical)
        {
        computeSphericalWeightedDensities(state);
        }
    else
        {
        // TODO: throw error
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
        
        // do points near edges first and start sending them
            {
            #ifdef FLYFT_OPENMP
            #pragma omp parallel for schedule(static) default(none) firstprivate(mesh) \
            shared(n0,n1,n2,n3,nv1,nv2,phi,dphi_dn0,dphi_dn1,dphi_dn2,dphi_dn3,dphi_dnv1,dphi_dnv2,compute_value)
            #endif
            for (int idx=0; idx < buffer_shape_; ++idx)
                {
                computePhiAndDerivatives(idx,
                                         phi,dphi_dn0,dphi_dn1,dphi_dn2,dphi_dn3,dphi_dnv1,dphi_dnv2,
                                         n0,n1,n2,n3,nv1,nv2,
                                         compute_value);
                computePhiAndDerivatives(mesh->shape()-buffer_shape_+idx,
                                         phi,dphi_dn0,dphi_dn1,dphi_dn2,dphi_dn3,dphi_dnv1,dphi_dnv2,
                                         n0,n1,n2,n3,nv1,nv2,
                                         compute_value);
                }

            auto comm = state->getMesh();
            comm->startSync(dphi_dn0_);
            comm->startSync(dphi_dn1_);
            comm->startSync(dphi_dn2_);
            comm->startSync(dphi_dn3_);
            comm->startSync(dphi_dnv1_);
            comm->startSync(dphi_dnv2_);
            }

        // do all the inside points
        #ifdef FLYFT_OPENMP
        #pragma omp parallel for schedule(static) default(none) firstprivate(mesh) \
        shared(n0,n1,n2,n3,nv1,nv2,phi,dphi_dn0,dphi_dn1,dphi_dn2,dphi_dn3,dphi_dnv1,dphi_dnv2,compute_value)
        #endif
        for (int idx=buffer_shape_; idx < mesh->shape()-buffer_shape_; ++idx)
            {
            computePhiAndDerivatives(idx,
                                     phi,dphi_dn0,dphi_dn1,dphi_dn2,dphi_dn3,dphi_dnv1,dphi_dnv2,
                                     n0,n1,n2,n3,nv1,nv2,
                                     compute_value);
            }

        // finish sending the data
            {
            state->getMesh()->endSync(dphi_dn0_);
            state->getMesh()->endSync(dphi_dn1_);
            state->getMesh()->endSync(dphi_dn2_);
            state->getMesh()->endSync(dphi_dn3_);
            state->getMesh()->endSync(dphi_dnv1_);
            state->getMesh()->endSync(dphi_dnv2_);
            }
        }

    // compute functional derivatives
    if (conv_type == ConvolutionType::cartesian)
        {
        computeCartesianDerivative(state);
        }
    else if (conv_type == ConvolutionType::spherical)
        {
        computeSphericalDerivative(state);
        }
    else
        {
        // TODO: throw error
        }

    // reduce the value of the functional if requested
    if (compute_value)
        {
        auto phi = phi_->const_view();
        value_ = 0.0;
        #ifdef FLYFT_OPENMP
        #pragma omp parallel for schedule(static) default(none) firstprivate(mesh) shared(phi) reduction(+:value_)
        #endif
        for (int idx=0; idx < mesh->shape(); ++idx)
            {
            value_ += mesh->integrateVolume(idx, phi);
            }
        value_ = state->getCommunicator()->sum(value_);
        }
    }

void RosenfeldFMT::computeCartesianWeightedDensities(std::shared_ptr<State> state)
    {
    const auto kmesh = ft_->getWavevectors();
    
    // zero the weights before accumulating by type
    std::fill(tmp_complex_field_["n0k"]->view().begin(), tmp_complex_field_["n0k"]->view().end(), 0.);
    std::fill(tmp_complex_field_["n1k"]->view().begin(), tmp_complex_field_["n1k"]->view().end(), 0.);
    std::fill(tmp_complex_field_["n2k"]->view().begin(), tmp_complex_field_["n2k"]->view().end(), 0.);
    std::fill(tmp_complex_field_["n3k"]->view().begin(), tmp_complex_field_["n3k"]->view().end(), 0.);
    std::fill(tmp_complex_field_["nv1k"]->view().begin(), tmp_complex_field_["nv1k"]->view().end(), 0.);
    std::fill(tmp_complex_field_["nv2k"]->view().begin(), tmp_complex_field_["nv2k"]->view().end(), 0.);

    for (const auto& t : state->getTypes())
        {
        // hard-sphere radius
        const double R = 0.5*diameters_(t);
        if (R == 0.)
            {
            // no radius, no weights contribute (skip)
            continue;
            }

        // fft the density
        ft_->setRealData(state->getField(t)->const_full_view());
        ft_->transform();
        auto rhok = ft_->const_view_reciprocal();

        // accumulate the fourier transformed densities into n
        auto n0k = tmp_complex_field_["n0k"]->view();
        auto n1k = tmp_complex_field_["n1k"]->view();
        auto n2k = tmp_complex_field_["n2k"]->view();
        auto n3k = tmp_complex_field_["n3k"]->view();
        auto nv1k = tmp_complex_field_["nv1k"]->view();
        auto nv2k = tmp_complex_field_["nv2k"]->view();
        #ifdef FLYFT_OPENMP
        #pragma omp parallel for schedule(static) default(none) firstprivate(R,kmesh) shared(rhok,n0k,n1k,n2k,n3k,nv1k,nv2k)
        #endif
        for (int idx=0; idx < kmesh.shape(); ++idx)
            {
            const double k = kmesh(idx);

            // compute weights at this k, using limiting values for k = 0
            std::complex<double> w0,w1,w2,w3,wv1,wv2;
            computeWeights(w2,w3,wv2,k,R);
            computeProportionalByWeight(w0, w1, wv1, w2, wv2, R);

            n0k(idx) += w0*rhok(idx);
            n1k(idx) += w1*rhok(idx);
            n2k(idx) += w2*rhok(idx);
            n3k(idx) += w3*rhok(idx);
            nv1k(idx) += wv1*rhok(idx);
            nv2k(idx) += wv2*rhok(idx);
            }
        }

    // transform n weights to real space to finish convolution
    // no need for a factor of mesh.step() here because w is analytical
    ft_->setReciprocalData(tmp_complex_field_["n0k"]->const_view());
    ft_->transform();
    std::copy(ft_->const_view_real().begin(),ft_->const_view_real().end(),n0_->full_view().begin());

    ft_->setReciprocalData(tmp_complex_field_["n1k"]->const_view());
    ft_->transform();
    std::copy(ft_->const_view_real().begin(),ft_->const_view_real().end(),n1_->full_view().begin());

    ft_->setReciprocalData(tmp_complex_field_["n2k"]->const_view());
    ft_->transform();
    std::copy(ft_->const_view_real().begin(),ft_->const_view_real().end(),n2_->full_view().begin());

    ft_->setReciprocalData(tmp_complex_field_["n3k"]->const_view());
    ft_->transform();
    std::copy(ft_->const_view_real().begin(),ft_->const_view_real().end(),n3_->full_view().begin());

    ft_->setReciprocalData(tmp_complex_field_["nv1k"]->const_view());
    ft_->transform();
    std::copy(ft_->const_view_real().begin(),ft_->const_view_real().end(),nv1_->full_view().begin());

    ft_->setReciprocalData(tmp_complex_field_["nv2k"]->const_view());
    ft_->transform();
    std::copy(ft_->const_view_real().begin(),ft_->const_view_real().end(),nv2_->full_view().begin());
    }

void RosenfeldFMT::computeSphericalWeightedDensities(std::shared_ptr<State> state)
    {
    const auto mesh = state->getMesh()->local().get();
    const auto kmesh = ft_->getWavevectors();

    // zero the real space weighted densities for accumulation later
    std::fill(n0_->view().begin(), n0_->view().end(), 0.);
    std::fill(n1_->view().begin(), n1_->view().end(), 0.);
    std::fill(n2_->view().begin(), n2_->view().end(), 0.);
    std::fill(n3_->view().begin(), n3_->view().end(), 0.);
    std::fill(nv1_->view().begin(), nv1_->view().end(), 0.);
    std::fill(nv2_->view().begin(), nv2_->view().end(), 0.);

    // weighted densities by type
    setupField(tmp_field_["n2"]);
    setupField(tmp_field_["n3"]);
    setupField(tmp_field_["nv2"]);

    for (const auto& t : state->getTypes())
        {
        // hard-sphere radius
        const double R = 0.5*diameters_(t);
        if (R == 0.)
            {
            // no radius, no weights contribute (skip)
            continue;
            }

        // get the fourier transformed weighted densities for this type
            {
            fourierTransformFieldSpherical(state->getField(t)->const_full_view(), mesh);
            auto rhok = ft_->const_view_reciprocal();
            auto n2k = tmp_complex_field_["n2k"]->view();
            auto n3k = tmp_complex_field_["n3k"]->view();
            auto nv2k = tmp_complex_field_["nv2k"]->view();
            #ifdef FLYFT_OPENMP
            #pragma omp parallel for schedule(static) default(none) firstprivate(R,kmesh) shared(rhok,n0k,n1k,n2k,n3k,nv1k,nv2k)
            #endif
            for (int idx=0; idx < kmesh.shape(); ++idx)
                {
                const double k = kmesh(idx);

                // compute weights at this k, using limiting values for k = 0
                std::complex<double> w2,w3,wv2;
                computeWeights(w2,w3,wv2,k,R);

                n2k(idx) = w2*rhok(idx);
                n3k(idx) = w3*rhok(idx);
                nv2k(idx) = wv2*rhok(idx);
                }
            }

        // transform n weights to real space to finish convolution
        // no need for a factor of mesh.step() here because w is analytical
        ft_->setReciprocalData(tmp_complex_field_["n2k"]->const_view());
        ft_->transform();
        std::copy(ft_->const_view_real().begin(),ft_->const_view_real().end(),tmp_field_["n2"]->full_view().begin());

        ft_->setReciprocalData(tmp_complex_field_["n3k"]->const_view());
        ft_->transform();
        std::copy(ft_->const_view_real().begin(),ft_->const_view_real().end(),tmp_field_["n3"]->full_view().begin());

        ft_->setReciprocalData(tmp_complex_field_["nv2k"]->const_view());
        ft_->transform();
        std::copy(ft_->const_view_real().begin(),ft_->const_view_real().end(),tmp_field_["nv2"]->full_view().begin());

        // divide through by r
            {
            auto n2i = tmp_field_["n2"]->view();
            auto n3i = tmp_field_["n3"]->view();
            auto nv2i = tmp_field_["nv2"]->view();
            for(int idx = 0; idx < mesh->shape(); ++idx)
                {           
                const auto r = mesh->center(idx);
                n2i(idx) /= r;
                n3i(idx) /= r;
                // nv2 also needs to have n3/r added, not just scale by r
                nv2i(idx) = nv2i(idx)/r + n3i(idx)/r;
                }
            }

        // fix up n for the r values that are close to the origin
        if (mesh->lower_bound(0) < R)
            {
            const auto rho = state->getField(t)->const_view(); 
            auto n2i = tmp_field_["n2"]->view();
            auto n3i = tmp_field_["n3"]->view();
            auto nv2i = tmp_field_["nv2"]->view();
            
            
            for (int idx=0; idx < std::min(mesh->bin(R), mesh->shape()); ++idx)
                {
                // reset n for this bin
                n2i(idx) = 0.;
                n3i(idx) = 0.;
                nv2i(idx) = 0.;
                    
                // take integrals using trapezoidal rule
                const auto r = mesh->center(idx);
    
                const double lower = 0;
                const double split = R-r;
                const double upper = r+R;
            
                // initializing n3 integrals 
                double n3_ig_1 = 0;
                double n3_ig_2 = 0; 
               
               
            
                // integral from 0 to R-r
                const int n = 100;
                double x = lower;
                double dr = (split-lower)/n;
                for (int ig_idx=0; ig_idx <=n; ig_idx++)
                    {
                    const double factor = (ig_idx == 0 || ig_idx == n) ? 0.5 : 1.0;
                    double rho_ig = mesh->interpolate(x, rho);
                    n3_ig_1 += factor * 4*M_PI* x * x * rho_ig;
                    x += dr;
                    }
                 n3_ig_1 *= dr;                
                 
                 //Integral from R-r to r+R
                 x = split;
                 dr = (upper-split)/n;

                for (int ig_idx=0; ig_idx <=n; ig_idx++)
                    {
                    const double factor = (ig_idx == 0 || ig_idx == n) ? 0.5 : 1.0;
                    const auto rho_ig = mesh->interpolate(x,rho);
                    n3_ig_2 += factor * (M_PI/r) * (rho_ig *x* (R*R - (r-x)*(r-x)));
                    n2i(idx) += factor * (2.*M_PI*R/r) * rho_ig * x;
                    nv2i(idx) += factor * (M_PI/(r*r)) * (rho_ig * x  * (R*R + r*r - x*x));
                    x +=dr;
                    }
                    
                n3i(idx) = n3_ig_1+ n3_ig_2 * dr;
                n2i(idx) *= dr;
                nv2i(idx) *= dr;
                }
            }

        // accumulate the weighted densities for the type into the total
            {
            auto n0 = n0_->view();
            auto n1 = n1_->view();
            auto n2 = n2_->view();
            auto n3 = n3_->view();
            auto nv1 = nv1_->view();
            auto nv2 = nv2_->view();

            auto n2i = tmp_field_["n2"]->const_view();
            auto n3i = tmp_field_["n3"]->const_view();
            auto nv2i = tmp_field_["nv2"]->const_view();
            
            for (int idx=0; idx < mesh->shape(); ++idx)
                {
                double n0i, n1i, nv1i;
                computeProportionalByWeight(n0i, n1i, nv1i, n2i(idx), nv2i(idx), R);

                n0(idx) += n0i;
                n1(idx) += n1i;
                n2(idx) += n2i(idx);
                n3(idx) += n3i(idx);
                nv1(idx) += nv1i;
                nv2(idx) += nv2i(idx);    
                }
            }
        }
    }

void RosenfeldFMT::computeCartesianDerivative(std::shared_ptr<State> state)
    {
    const auto mesh = state->getMesh()->local().get();
    const auto kmesh = ft_->getWavevectors();

    // convert phi derivatives to Fourier space
        {
        ft_->setRealData(dphi_dn0_->const_full_view());
        ft_->transform();
        std::copy(ft_->const_view_reciprocal().begin(),ft_->const_view_reciprocal().end(),dphi_dn0k_->view().begin());

        ft_->setRealData(dphi_dn1_->const_full_view());
        ft_->transform();
        std::copy(ft_->const_view_reciprocal().begin(),ft_->const_view_reciprocal().end(),dphi_dn1k_->view().begin());

        ft_->setRealData(dphi_dn2_->const_full_view());
        ft_->transform();
        std::copy(ft_->const_view_reciprocal().begin(),ft_->const_view_reciprocal().end(),dphi_dn2k_->view().begin());

        ft_->setRealData(dphi_dn3_->const_full_view());
        ft_->transform();
        std::copy(ft_->const_view_reciprocal().begin(),ft_->const_view_reciprocal().end(),dphi_dn3k_->view().begin());

        ft_->setRealData(dphi_dnv1_->const_full_view());
        ft_->transform();
        std::copy(ft_->const_view_reciprocal().begin(),ft_->const_view_reciprocal().end(),dphi_dnv1k_->view().begin());

        ft_->setRealData(dphi_dnv2_->const_full_view());
        ft_->transform();
        std::copy(ft_->const_view_reciprocal().begin(),ft_->const_view_reciprocal().end(),dphi_dnv2k_->view().begin());
        }

    // convolve phi derivatives with weights to get functional derivatives
    // again, no need for a factor of mesh.step() here because w is analytical
        {
        auto dphi_dn0k = dphi_dn0k_->const_view();
        auto dphi_dn1k = dphi_dn1k_->const_view();
        auto dphi_dn2k = dphi_dn2k_->const_view();
        auto dphi_dn3k = dphi_dn3k_->const_view();
        auto dphi_dnv1k = dphi_dnv1k_->const_view();
        auto dphi_dnv2k = dphi_dnv2k_->const_view();

        auto derivativek = derivativek_->view();
        for (const auto& t : state->getTypes())
            {
            // hard-sphere radius
            const double R = 0.5*diameters_(t);
            if (R == 0.)
                {
                // no radius, no contribution to energy
                // need to set here as we are not prefilling the array with zeros
                auto derivative = derivatives_(t)->view();
                std::fill(derivative.begin(), derivative.end(),0.0);
                continue;
                }

            #ifdef FLYFT_OPENMP
            #pragma omp parallel for schedule(static) default(none) firstprivate(kmesh,R) \
            shared(derivativek,dphi_dn0k,dphi_dn1k,dphi_dn2k,dphi_dn3k,dphi_dnv1k,dphi_dnv2k)
            #endif
            for (int idx=0; idx < kmesh.shape(); ++idx)
                {
                const double k = kmesh(idx);

                // get weights
                std::complex<double> w0,w1,w2,w3,wv1,wv2;
                computeWeights(w2,w3,wv2,k,R);
                computeProportionalByWeight(w0, w1, wv1, w2, wv2, R);

                // convolution (note opposite sign for vector weights due to change of order in convolution)
                derivativek(idx) = (dphi_dn0k(idx)*w0+dphi_dn1k(idx)*w1+dphi_dn2k(idx)*w2+dphi_dn3k(idx)*w3
                                    -dphi_dnv1k(idx)*wv1-dphi_dnv2k(idx)*wv2);
                }
            ft_->setReciprocalData(derivativek);
            ft_->transform();

            // copy the valid values
            Field::ConstantView din(ft_->const_view_real().begin().get(),
                                    DataLayout(ft_->getMesh().shape()),
                                    buffer_shape_,
                                    buffer_shape_+mesh->shape());
            auto dout = derivatives_(t)->view();
            #ifdef FLYFT_OPENMP
            #pragma omp parallel for schedule(static) default(none) firstprivate(mesh) shared(din,dout)
            #endif
            for (int idx=0; idx < mesh->shape(); ++idx)
                {
                dout(idx) = din(idx);
                }

            // start communicating this type
            state->getMesh()->startSync(derivatives_(t));
            }
        }

    // finish up communication of all types
    for (const auto& t : state->getTypes())
        {
        state->getMesh()->endSync(derivatives_(t));
        }
    }

void RosenfeldFMT::computeSphericalDerivative(std::shared_ptr<State> state)
    {
    const auto mesh = state->getMesh()->local().get();
    const auto kmesh = ft_->getWavevectors();  

    // these temporary variables will be used for convolutions of each derivative per type
    setupField(tmp_field_["dF_dn0"]);
    setupField(tmp_field_["dF_dn1"]);
    setupField(tmp_field_["dF_dn2"]);
    setupField(tmp_field_["dF_dn3"]);
    setupField(tmp_field_["dF_dnv1"]);
    setupField(tmp_field_["dF_dnv2"]);
    setupField(tmp_field_["dF_dnv2_2"]);
    setupField(tmp_field_["dF_dnv1_2"]);
    
    setupComplexField(tmp_complex_field_["dphi_dn0k_w0k"]);
    setupComplexField(tmp_complex_field_["dphi_dn1k_w1k"]);
    setupComplexField(tmp_complex_field_["dphi_dn2k_w2k"]);
    setupComplexField(tmp_complex_field_["dphi_dn3k_w3k"]);
    setupComplexField(tmp_complex_field_["dphi_dnv1k_wv1k"]);
    setupComplexField(tmp_complex_field_["dphi_dnv2k_wv2k"]);
    setupComplexField(tmp_complex_field_["dphi_dnv2k_w3k"]);
    setupComplexField(tmp_complex_field_["dphi_dnv1k_w3k"]);
    // convert phi derivatives to Fourier space for convolution, accounting for factor of r
    // these can be reused by all of the types, so we compute them first outside the type loop
        {
        // dphi_dn0
        fourierTransformFieldSpherical(dphi_dn0_->const_full_view(), mesh);
        std::copy(ft_->const_view_reciprocal().begin(), ft_->const_view_reciprocal().end(), dphi_dn0k_->full_view().begin());

        // dphi_dn1
        fourierTransformFieldSpherical(dphi_dn1_->const_full_view(), mesh);
        std::copy(ft_->const_view_reciprocal().begin(), ft_->const_view_reciprocal().end(), dphi_dn1k_->full_view().begin());

        // dphi_dn2
        fourierTransformFieldSpherical(dphi_dn2_->const_full_view(), mesh);
        std::copy(ft_->const_view_reciprocal().begin(), ft_->const_view_reciprocal().end(), dphi_dn2k_->full_view().begin());

        // dphi_dn3
        fourierTransformFieldSpherical(dphi_dn3_->const_full_view(), mesh);
        std::copy(ft_->const_view_reciprocal().begin(), ft_->const_view_reciprocal().end(), dphi_dn3k_->full_view().begin());

        // dphi_dnv1
        fourierTransformFieldSpherical(dphi_dnv1_->const_full_view(), mesh);
        std::copy(ft_->const_view_reciprocal().begin(), ft_->const_view_reciprocal().end(), dphi_dnv1k_->full_view().begin());

        // dphi_dnv2
        fourierTransformFieldSpherical(dphi_dnv2_->const_full_view(), mesh);
        std::copy(ft_->const_view_reciprocal().begin(), ft_->const_view_reciprocal().end(), dphi_dnv2k_->full_view().begin());
        }
    
    for (const auto& t : state->getTypes())
        {
        // hard-sphere radius
        const double R = 0.5*diameters_(t);
        if (R == 0.)
            {
            // no radius, no contribution to energy
            // need to set here as we are not prefilling the array with zeros
            auto derivative = derivatives_(t)->view();
            std::fill(derivative.begin(), derivative.end(), 0.0);
            continue;
            }

        // convolve phi derivatives with weight function
            {
            auto dphi_dn0k = dphi_dn0k_->const_view();
            auto dphi_dn1k = dphi_dn1k_->const_view();
            auto dphi_dn2k = dphi_dn2k_->const_view();
            auto dphi_dn3k = dphi_dn3k_->const_view();
            auto dphi_dnv1k = dphi_dnv1k_->const_view();
            auto dphi_dnv2k = dphi_dnv2k_->const_view();
            auto dphi_dn0k_w0k = tmp_complex_field_["dphi_dn0k_w0k"]->view();
            auto dphi_dn1k_w1k = tmp_complex_field_["dphi_dn1k_w1k"]->view();
            auto dphi_dn2k_w2k = tmp_complex_field_["dphi_dn2k_w2k"]->view();
            auto dphi_dn3k_w3k = tmp_complex_field_["dphi_dn3k_w3k"]->view();
            auto dphi_dnv1k_wv1k = tmp_complex_field_["dphi_dnv1k_wv1k"]->view();
            auto dphi_dnv2k_wv2k = tmp_complex_field_["dphi_dnv2k_wv2k"]->view();
            auto dphi_dnv2k_w3k = tmp_complex_field_["dphi_dnv2k_w3k"]->view();
            auto dphi_dnv1k_w3k = tmp_complex_field_["dphi_dnv1k_w3k"]->view();
            
            for (int idx = 0; idx < kmesh.shape(); ++idx)
                {
                const double k = kmesh(idx);

                std::complex<double> w0,w1,w2,w3,wv1,wv2;
                computeWeights(w2,w3,wv2,k,R);
                computeProportionalByWeight(w0, w1, wv1, w2, wv2, R);

                dphi_dn0k_w0k(idx) = dphi_dn0k(idx)*w0;
                dphi_dn1k_w1k(idx) = dphi_dn1k(idx)*w1;
                dphi_dn2k_w2k(idx) = dphi_dn2k(idx)*w2;
                dphi_dn3k_w3k(idx) = dphi_dn3k(idx)*w3;
                // sign is opposite here due to oddness of weight function and reversed order
                dphi_dnv1k_wv1k(idx) = -dphi_dnv1k(idx)*wv1;
                dphi_dnv2k_wv2k(idx) = -dphi_dnv2k(idx)*wv2;
                // TODO: there is a missing term for nv1 and nv2
                dphi_dnv2k_w3k(idx) = -dphi_dnv2k(idx)*w3;
                dphi_dnv1k_w3k(idx) = -dphi_dnv1k(idx)*w3;
                }

            ft_->setReciprocalData(dphi_dn0k_w0k);
            ft_->transform();
            std::copy(ft_->const_view_real().begin(),ft_->const_view_real().end(),tmp_field_["dF_dn0"]->full_view().begin());

            ft_->setReciprocalData(dphi_dn1k_w1k);
            ft_->transform();
            std::copy(ft_->const_view_real().begin(),ft_->const_view_real().end(),tmp_field_["dF_dn1"]->full_view().begin());

            ft_->setReciprocalData(dphi_dn2k_w2k);
            ft_->transform();
            std::copy(ft_->const_view_real().begin(),ft_->const_view_real().end(),tmp_field_["dF_dn2"]->full_view().begin());

            ft_->setReciprocalData(dphi_dn3k_w3k);
            ft_->transform();
            std::copy(ft_->const_view_real().begin(),ft_->const_view_real().end(),tmp_field_["dF_dn3"]->full_view().begin());

            ft_->setReciprocalData(dphi_dnv1k_wv1k);
            ft_->transform();
            std::copy(ft_->const_view_real().begin(),ft_->const_view_real().end(),tmp_field_["dF_dnv1"]->full_view().begin());
            
            ft_->setReciprocalData(dphi_dnv2k_wv2k);
            ft_->transform();
            std::copy(ft_->const_view_real().begin(),ft_->const_view_real().end(),tmp_field_["dF_dnv2"]->full_view().begin());
            
            ft_->setReciprocalData(dphi_dnv2k_w3k);
            ft_->transform();
            std::copy(ft_->const_view_real().begin(),ft_->const_view_real().end(),tmp_field_["dF_dnv2_2"]->full_view().begin());
            
            ft_->setReciprocalData(dphi_dnv1k_w3k);
            ft_->transform();
            std::copy(ft_->const_view_real().begin(),ft_->const_view_real().end(),tmp_field_["dF_dnv2_2"]->full_view().begin());
            }
            
        
        // normalize phi derivatives with r
            {        
            auto dF_dn0 = tmp_field_["dF_dn0"]->view();
            auto dF_dn1 = tmp_field_["dF_dn1"]->view();
            auto dF_dn2 = tmp_field_["dF_dn2"]->view();
            auto dF_dn3 = tmp_field_["dF_dn3"]->view();
            auto dF_dnv1 = tmp_field_["dF_dnv1"]->view();
            auto dF_dnv2 = tmp_field_["dF_dnv2"]->view();
            auto dF_dnv2_2 = tmp_field_["dF_dnv2_2"]->view();
            auto dF_dnv1_2 = tmp_field_["dF_dnv1_2"]->view();
            
            for(int idx = 0; idx < mesh->shape(); ++idx)
                {           
                const auto r = mesh->center(idx);
                dF_dn0(idx) /= r;
                dF_dn1(idx) /= r;
                dF_dn2(idx) /= r;
                dF_dn3(idx) /= r;
                dF_dnv1(idx) /= r;
                dF_dnv2(idx) /= r;
                // TODO: extra term for nv1 and nv2
                dF_dnv2(idx) += dF_dnv2_2(idx)/(r*r);
                dF_dnv1(idx) += dF_dnv1_2(idx)/(r*r);
                }
            }

        // handle special case points near center
        if (mesh->lower_bound(0) < R)
            {
            auto dphi_dn0 = dphi_dn0_->const_view();
            auto dphi_dn1 = dphi_dn1_->const_view();
            auto dphi_dn2 = dphi_dn2_->const_view();
            auto dphi_dn3 = dphi_dn3_->const_view();
            auto dphi_dnv1 = dphi_dnv1_->const_view();
            auto dphi_dnv2 = dphi_dnv2_->const_view();

            auto dF_dn0 = tmp_field_["dF_dn0"]->view();
            auto dF_dn1 = tmp_field_["dF_dn1"]->view();
            auto dF_dn2 = tmp_field_["dF_dn2"]->view();
            auto dF_dn3 = tmp_field_["dF_dn3"]->view();
            auto dF_dnv1 = tmp_field_["dF_dnv1"]->view();
            auto dF_dnv2 = tmp_field_["dF_dnv2"]->view();

            for (int idx=0; idx < std::min(mesh->bin(R), mesh->shape()); ++idx)
                {
                // reset values for this bin
                dF_dn0(idx) = 0.;
                dF_dn1(idx) = 0.;
                dF_dn2(idx) = 0.;
                dF_dn3(idx) = 0.;
                dF_dnv1(idx) = 0.;
                dF_dnv2(idx) = 0.;
                
                double dF_dn3_1 = 0;
                double dF_dn3_2 = 0;
                // take integrals using quadrature
                const auto r = mesh->center(idx);
                const double lower = 0.;
                const double split = R-r;
                const double upper = r+R;
                const double sq_R = R*R;
                const double sq_r = r*r;
                
                //integral 1
                //we have found 100 points works well

                const int n = 100;
                double dr = (split-lower)/n;
                double x = lower;
                
                for (int ig_idx=0; ig_idx <=n; ++ig_idx)
                    {
                    const auto dphi_dn3_ig = mesh->interpolate(x, dphi_dn3);
                    const double factor = (ig_idx == 0 || ig_idx == n) ? 0.5 : 1.0;
                    dF_dn3_1 += factor * 4. * M_PI * dphi_dn3_ig * (x * x) ;
                    x += dr;
                    }
                    dF_dn3_1 *= dr;           

                //Integral 2
                dr = (upper-split)/n;
                x = split;                
                
                for (int ig_idx=0; ig_idx <=n; ++ig_idx)
                    {
                    const double kernel_w3 = x * (M_PI/r) * (sq_R - (r-x)*(r-x));
                    const double kernel_w2 = x * (2.*M_PI*R/r);
                    // minus sign for wv2 kernel to account for swap of variables in convolution with odd function
                    const double kernel_wv2 = -x * (M_PI/(sq_r)) * (sq_R + sq_r - x*x);
                    double kernel_w0, kernel_w1, kernel_wv1;
                    computeProportionalByWeight(kernel_w0, kernel_w1, kernel_wv1, kernel_w2, kernel_wv2, R);
                    
                    const double factor = (ig_idx == 0 || ig_idx == n) ? 0.5 : 1.0;
                    const auto dphi_dn0_ig = mesh->interpolate(x, dphi_dn0);
                    const auto dphi_dn1_ig = mesh->interpolate(x, dphi_dn1);
                    const auto dphi_dn2_ig = mesh->interpolate(x, dphi_dn2);
                    const auto dphi_dn3_ig = mesh->interpolate(x, dphi_dn3);
                    const auto dphi_dnv1_ig = mesh->interpolate(x, dphi_dnv1);
                    const auto dphi_dnv2_ig = mesh->interpolate(x, dphi_dnv2);
                    

                    dF_dn0(idx) += factor * dphi_dn0_ig * kernel_w0;
                    dF_dn1(idx) += factor * dphi_dn1_ig * kernel_w1 ;
                    dF_dn2(idx) += factor * dphi_dn2_ig * kernel_w2 ;
                    dF_dn3_2 += factor * dphi_dn3_ig * kernel_w3 ;
                    dF_dnv1(idx) += factor * dphi_dnv1_ig * kernel_wv1 ;
                    dF_dnv2(idx) += factor * dphi_dnv2_ig* kernel_wv2 ;
                    x += dr;               
                    }
                
                dF_dn0(idx) *= dr;
                dF_dn1(idx) *= dr;
                dF_dn2(idx) *= dr;
                dF_dn3(idx) = dF_dn3_1+dF_dn3_2*dr; 
                dF_dnv1(idx) *= dr;
                dF_dnv2(idx) *= dr;          
                }
            }

        // combinine terms of the derivatives together
            {
            auto derivative = derivatives_(t)->view();

            auto dF_dn0 = tmp_field_["dF_dn0"]->const_view();
            auto dF_dn1 = tmp_field_["dF_dn1"]->const_view();
            auto dF_dn2 = tmp_field_["dF_dn2"]->const_view();
            auto dF_dn3 = tmp_field_["dF_dn3"]->const_view();
            auto dF_dnv1 = tmp_field_["dF_dnv1"]->const_view();
            auto dF_dnv2 = tmp_field_["dF_dnv2"]->const_view();

            #ifdef FLYFT_OPENMP
            #pragma omp parallel for schedule(static) default(none) firstprivate(mesh) \
            shared(derivative,dF_dn0,dF_dn1,dF_dn2,dF_dn3,dF_dnv1,dF_dnv2)
            #endif
            for (int idx=0; idx < mesh->shape(); ++idx)
                {
                derivative(idx) = dF_dn0(idx) + dF_dn1(idx) + dF_dn2(idx) + dF_dn3(idx) + dF_dnv1(idx) + dF_dnv2(idx);
                }
            }

        // start communicating this type
        state->getMesh()->startSync(derivatives_(t));
        }

    // finish up communication of all types
    for (const auto& t : state->getTypes())
        {
        state->getMesh()->endSync(derivatives_(t));
        }
    }

void RosenfeldFMT::computePrefactorFunctions(double& f1,
                                             double& f2,
                                             double& f4,
                                             double& df1dn3,
                                             double& df2dn3,
                                             double& df4dn3,
                                             double n3) const
    {
    // precompute the "void fraction" vf, which is only a function of n3
    const double vf = 1.-n3;
    if (vf < 0)
        {
        // local void fraction unphysical
        }
    const double logvf = std::log(vf);
    const double vfinv = 1./vf;
    f1 = -logvf;
    f2 = vfinv;
    f4 = vfinv*vfinv/(24.*M_PI);
    df1dn3 = vfinv;
    df2dn3 = vfinv*vfinv;
    df4dn3 = 2.*f4*vfinv;
    }

void RosenfeldFMT::computeWeights(std::complex<double>& w2,
                                  std::complex<double>& w3,
                                  std::complex<double>& wv2,
                                  double k,
                                  double R) const

    {
    if (k != 0.)
        {
        const double kR = k*R;
        const double sinkR = std::sin(kR);
        const double coskR = std::cos(kR);
        w2 = (4.*M_PI*R)*(sinkR/k);
        w3 = (4.*M_PI)*(sinkR-kR*coskR)/(k*k*k);
        wv2 = std::complex<double>(0.,-k)*w3;
        }
    else
        {
        w2 = 4.*M_PI*R*R;
        w3 = w2*(R/3.);
        wv2 = 0.0;
        }
    }
    
void RosenfeldFMT::computePhiAndDerivatives(int idx,
                                            Field::View& phi,
                                            Field::View& dphi_dn0,
                                            Field::View& dphi_dn1,
                                            Field::View& dphi_dn2,
                                            Field::View& dphi_dn3,
                                            Field::View& dphi_dnv1,
                                            Field::View& dphi_dnv2,
                                            const Field::ConstantView& n0,
                                            const Field::ConstantView& n1,
                                            const Field::ConstantView& n2,
                                            const Field::ConstantView& n3,
                                            const Field::ConstantView& nv1,
                                            const Field::ConstantView& nv2,
                                            bool compute_value) const
    {
    // these are all functions of n3 (via vf)
    double f1, f2, f4, df1, df2, df4;
    computePrefactorFunctions(f1, f2, f4, df1, df2, df4, n3(idx));

    if (compute_value)
        {
        phi(idx) = (f1*n0(idx)
                    +f2*(n1(idx)*n2(idx)-nv1(idx)*nv2(idx))
                    +f4*(n2(idx)*n2(idx)*n2(idx)-3.*n2(idx)*nv2(idx)*nv2(idx)));
        }
    else
        {
        phi(idx) = 0.;
        }

    dphi_dn0(idx) = f1;
    dphi_dn1(idx) = f2*n2(idx);
    dphi_dn2(idx) = f2*n1(idx) + 3.*f4*(n2(idx)*n2(idx)-nv2(idx)*nv2(idx));
    dphi_dn3(idx) = (df1*n0(idx)
                        +df2*(n1(idx)*n2(idx)-nv1(idx)*nv2(idx))
                        +df4*(n2(idx)*n2(idx)*n2(idx)-3.*n2(idx)*nv2(idx)*nv2(idx)));
    dphi_dnv1(idx) = -f2*nv2(idx);
    dphi_dnv2(idx) = -f2*nv1(idx)-6.*f4*n2(idx)*nv2(idx);
    }

int RosenfeldFMT::determineBufferShape(std::shared_ptr<State> state, const std::string& /*type*/)
    {
    // TODO: cache this
    double max_buffer = 0.0;
    for (const auto& t : state->getTypes())
        {
        max_buffer = std::max(max_buffer,diameters_(t));
        }
    buffer_shape_ = state->getMesh()->local()->asShape(max_buffer);
    return buffer_shape_;
    }

bool RosenfeldFMT::setup(std::shared_ptr<State> state, bool compute_value)
    {
    // this will setup buffer_shape_ indirectly
    bool compute = Functional::setup(state,compute_value);

    // update Fourier transform to mesh shape + buffer
    const auto mesh = state->getMesh()->local().get();
    const int buffered_shape = mesh->shape()+2*buffer_shape_;
    CartesianMesh ft_mesh(mesh->asLength(buffered_shape),buffered_shape,-0.5*mesh->step());
    if (!ft_ || ft_mesh != ft_->getMesh())
        {
        ft_ = std::make_unique<FourierTransform>(ft_mesh.L(),ft_mesh.shape());
        }

    // update shape of internal fields
    setupField(n0_); setupComplexField(tmp_complex_field_["n0k"]); setupField(dphi_dn0_); setupComplexField(dphi_dn0k_);
    setupField(n1_); setupComplexField(tmp_complex_field_["n1k"]);setupField(dphi_dn1_); setupComplexField(dphi_dn1k_);
    setupField(n2_); setupComplexField(tmp_complex_field_["n2k"]); setupField(dphi_dn2_); setupComplexField(dphi_dn2k_);
    setupField(n3_); setupComplexField(tmp_complex_field_["n3k"]); setupField(dphi_dn3_); setupComplexField(dphi_dn3k_);
    setupField(nv1_);setupComplexField(tmp_complex_field_["nv1k"]);setupField(dphi_dnv1_); setupComplexField(dphi_dnv1k_);
    setupField(nv2_);setupComplexField(tmp_complex_field_["nv2k"]);  setupField(dphi_dnv2_); setupComplexField(dphi_dnv2k_);

    // these two fields only exist in one space
    setupField(phi_);
    setupComplexField(derivativek_);

    return compute;
    }

void RosenfeldFMT::setupField(std::shared_ptr<Field>& field)
    {
    if (!field)
        {
        field = std::make_shared<Field>(ft_->getMesh().shape()-2*buffer_shape_,buffer_shape_);
        }
    else
        {
        field->reshape(ft_->getMesh().shape()-2*buffer_shape_,buffer_shape_);
        }
    }

void RosenfeldFMT::setupComplexField(std::unique_ptr<ComplexField>& kfield)
    {
    if (!kfield)
        {
        kfield = std::make_unique<ComplexField>(ft_->getWavevectors().shape(),0);
        }
    else
        {
        kfield->reshape(ft_->getWavevectors().shape(),0);
        }
    }

RosenfeldFMT::ConvolutionType RosenfeldFMT::getConvolutionType(std::shared_ptr<const Mesh> mesh) const
    {
    const Mesh* mesh_raw = mesh.get();
    ConvolutionType conv_type;
    if (dynamic_cast<const CartesianMesh*>(mesh_raw) != nullptr)
        {
        conv_type = ConvolutionType::cartesian;
        }
    else if(dynamic_cast<const SphericalMesh*>(mesh_raw) != nullptr)
        {
        conv_type = ConvolutionType::spherical;
        }
    else
        {
        throw std::runtime_error("Failed to deduce type");
        }
    return conv_type;
    }

void RosenfeldFMT::fourierTransformFieldSpherical(const Field::ConstantView& input,
                                                  const Mesh* mesh) const
    {
    auto tmp = tmp_r_field_->full_view();
    for (int idx=0; idx < mesh->shape() + 2*buffer_shape_; ++idx)
        {
        // r can't be negative, setting it to zero effectively throws this term out, as it should
        const auto r = std::max(mesh->center(idx-buffer_shape_), 0.);
        tmp(idx) = r * input(idx);
        }
    ft_->setRealData(tmp);
    ft_->transform();
    }

TypeMap<double>& RosenfeldFMT::getDiameters()
    {
    return diameters_;
    }

const TypeMap<double>& RosenfeldFMT::getDiameters() const
    {
    return diameters_;
    }

}
