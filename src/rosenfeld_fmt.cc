#include "flyft/rosenfeld_fmt.h"

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

static void computePhiAndDerivatives(int idx,
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
                                     bool compute_value)
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

static void computeWeights(std::complex<double>& w0,
                           std::complex<double>& w1,
                           std::complex<double>& w2,
                           std::complex<double>& w3,
                           std::complex<double>& wv1,
                           std::complex<double>& wv2,
                           double k,
                           double R)

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

void RosenfeldFMT::compute(std::shared_ptr<State> state, bool compute_value)
    {
    bool needs_compute = setup(state,compute_value);
    if (!needs_compute)
        {
        return;
        }

    // compute n weights in fourier space (requires communication before fourier transform)
    state->syncFields();
    const auto mesh = *state->getMesh()->local();
    const auto kmesh = ft_->getWavevectors();
        {
        // zero the weights before accumulating by type
        std::fill(n0k_->view().begin(), n0k_->view().end(), 0.);
        std::fill(n1k_->view().begin(), n1k_->view().end(), 0.);
        std::fill(n2k_->view().begin(), n2k_->view().end(), 0.);
        std::fill(n3k_->view().begin(), n3k_->view().end(), 0.);
        std::fill(nv1k_->view().begin(), nv1k_->view().end(), 0.);
        std::fill(nv2k_->view().begin(), nv2k_->view().end(), 0.);

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
            auto n0k = n0k_->view();
            auto n1k = n1k_->view();
            auto n2k = n2k_->view();
            auto n3k = n3k_->view();
            auto nv1k = nv1k_->view();
            auto nv2k = nv2k_->view();
            #ifdef FLYFT_OPENMP
            #pragma omp parallel for schedule(static) default(none) firstprivate(R,kmesh) shared(rhok,n0k,n1k,n2k,n3k,nv1k,nv2k)
            #endif
            for (int idx=0; idx < kmesh.shape(); ++idx)
                {
                const double k = kmesh(idx);

                // compute weights at this k, using limiting values for k = 0
                std::complex<double> w0,w1,w2,w3,wv1,wv2;
                computeWeights(w0,w1,w2,w3,wv1,wv2,k,R);

                n0k(idx) += w0*rhok(idx);
                n1k(idx) += w1*rhok(idx);
                n2k(idx) += w2*rhok(idx);
                n3k(idx) += w3*rhok(idx);
                nv1k(idx) += wv1*rhok(idx);
                nv2k(idx) += wv2*rhok(idx);
                }
            }
        }

    // transform n weights to real space to finish convolution
    // no need for a factor of mesh.step() here because w is analytical
        {
        ft_->setReciprocalData(n0k_->const_view());
        ft_->transform();
        std::copy(ft_->const_view_real().begin(),ft_->const_view_real().end(),n0_->full_view().begin());

        ft_->setReciprocalData(n1k_->const_view());
        ft_->transform();
        std::copy(ft_->const_view_real().begin(),ft_->const_view_real().end(),n1_->full_view().begin());

        ft_->setReciprocalData(n2k_->const_view());
        ft_->transform();
        std::copy(ft_->const_view_real().begin(),ft_->const_view_real().end(),n2_->full_view().begin());

        ft_->setReciprocalData(n3k_->const_view());
        ft_->transform();
        std::copy(ft_->const_view_real().begin(),ft_->const_view_real().end(),n3_->full_view().begin());

        ft_->setReciprocalData(nv1k_->const_view());
        ft_->transform();
        std::copy(ft_->const_view_real().begin(),ft_->const_view_real().end(),nv1_->full_view().begin());

        ft_->setReciprocalData(nv2k_->const_view());
        ft_->transform();
        std::copy(ft_->const_view_real().begin(),ft_->const_view_real().end(),nv2_->full_view().begin());
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
                computePhiAndDerivatives(mesh.shape()-buffer_shape_+idx,
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
        for (int idx=buffer_shape_; idx < mesh.shape()-buffer_shape_; ++idx)
            {
            computePhiAndDerivatives(idx,
                                     phi,dphi_dn0,dphi_dn1,dphi_dn2,dphi_dn3,dphi_dnv1,dphi_dnv2,
                                     n0,n1,n2,n3,nv1,nv2,
                                     compute_value);
            }

        // finish sending the data
            {
            auto comm = state->getMesh();
            comm->endSync(dphi_dn0_);
            comm->endSync(dphi_dn1_);
            comm->endSync(dphi_dn2_);
            comm->endSync(dphi_dn3_);
            comm->endSync(dphi_dnv1_);
            comm->endSync(dphi_dnv2_);
            }
        }

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
                computeWeights(w0,w1,w2,w3,wv1,wv2,k,R);

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
                                    buffer_shape_+mesh.shape());
            auto dout = derivatives_(t)->view();
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
    if (compute_value)
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
        value_ = state->getCommunicator()->sum(value_);
        }

    finalize(state,compute_value);
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
    const auto mesh = *state->getMesh()->local();
    const int buffered_shape = mesh.shape()+2*buffer_shape_;
    Mesh ft_mesh(mesh.asLength(buffered_shape),buffered_shape,-0.5*mesh.step());
    if (!ft_ || ft_mesh != ft_->getMesh())
        {
        ft_ = std::make_unique<FourierTransform>(ft_mesh.L(),ft_mesh.shape());
        }

    // update shape of internal fields
    setupField(n0_); setupComplexField(n0k_); setupField(dphi_dn0_); setupComplexField(dphi_dn0k_);
    setupField(n1_); setupComplexField(n1k_); setupField(dphi_dn1_); setupComplexField(dphi_dn1k_);
    setupField(n2_); setupComplexField(n2k_); setupField(dphi_dn2_); setupComplexField(dphi_dn2k_);
    setupField(n3_); setupComplexField(n3k_); setupField(dphi_dn3_); setupComplexField(dphi_dn3k_);
    setupField(nv1_); setupComplexField(nv1k_); setupField(dphi_dnv1_); setupComplexField(dphi_dnv1k_);
    setupField(nv2_); setupComplexField(nv2k_); setupField(dphi_dnv2_); setupComplexField(dphi_dnv2k_);

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

TypeMap<double>& RosenfeldFMT::getDiameters()
    {
    return diameters_;
    }

const TypeMap<double>& RosenfeldFMT::getDiameters() const
    {
    return diameters_;
    }

}
