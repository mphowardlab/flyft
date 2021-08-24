#include "flyft/fourier_transform.h"

#include <algorithm>
#ifdef FLYFT_OPENMP
#include <omp.h>
#endif

namespace flyft
{
FourierTransform::FourierTransform(double L, int shape)
    : mesh_(L,shape,-0.5*L/shape),
      kmesh_(L,shape),
      space_(RealSpace)
    {
    #ifdef FLYFT_OPENMP
    // use all available OpenMP threads
    fftw_init_threads();
    fftw_plan_with_nthreads(omp_get_max_threads());
    #endif

    // this is the doc'd size of "real" memory required for the r2c / c2r transform
    data_ = fftw_alloc_real(2*(mesh_.shape()/2+1));

    r2c_plan_ = fftw_plan_dft_r2c_1d(mesh_.shape(),
                                     data_,
                                     reinterpret_cast<fftw_complex*>(data_),
                                     FFTW_ESTIMATE);

    c2r_plan_ = fftw_plan_dft_c2r_1d(mesh_.shape(),
                                     reinterpret_cast<fftw_complex*>(data_),
                                     data_,
                                     FFTW_ESTIMATE);
    }

FourierTransform::~FourierTransform()
    {
    if (data_) fftw_free(data_);
    fftw_destroy_plan(r2c_plan_);
    fftw_destroy_plan(c2r_plan_);
    }

FourierTransform::RealView FourierTransform::view_real() const
    {
    if (space_ != RealSpace)
        {
        // raise error, buffer not valid
        }
    return RealView(data_,DataLayout(mesh_.shape()));
    }

FourierTransform::ConstantRealView FourierTransform::const_view_real() const
    {
    if (space_ != RealSpace)
        {
        // raise error, buffer not valid
        }
    return ConstantRealView(data_,DataLayout(mesh_.shape()));
    }

void FourierTransform::setRealData(const RealView& data)
    {
    RealView view(data_,DataLayout(mesh_.shape()));
    std::copy(data.begin(),data.end(),view.begin());
    space_ = RealSpace;
    }

void FourierTransform::setRealData(const ConstantRealView& data)
    {
    RealView view(data_,DataLayout(mesh_.shape()));
    std::copy(data.begin(),data.end(),view.begin());
    space_ = RealSpace;
    }

FourierTransform::ReciprocalView FourierTransform::view_reciprocal() const
    {
    if (space_ != ReciprocalSpace)
        {
        // raise error, buffer not valid
        }
    return ReciprocalView(reinterpret_cast<std::complex<double>*>(data_),DataLayout(kmesh_.shape()));
    }

FourierTransform::ConstantReciprocalView FourierTransform::const_view_reciprocal() const
    {
    if (space_ != ReciprocalSpace)
        {
        // raise error, buffer not valid
        }
    return ConstantReciprocalView(reinterpret_cast<const std::complex<double>*>(data_),DataLayout(kmesh_.shape()));
    }

void FourierTransform::setReciprocalData(const ReciprocalView& data)
    {
    ReciprocalView view(reinterpret_cast<std::complex<double>*>(data_),DataLayout(kmesh_.shape()));
    std::copy(data.begin(),data.end(),view.begin());
    space_ = ReciprocalSpace;
    }

void FourierTransform::setReciprocalData(const ConstantReciprocalView& data)
    {
    ReciprocalView view(reinterpret_cast<std::complex<double>*>(data_),DataLayout(kmesh_.shape()));
    std::copy(data.begin(),data.end(),view.begin());
    space_ = ReciprocalSpace;
    }

FourierTransform::Space FourierTransform::getActiveSpace() const
    {
    return space_;
    }

void FourierTransform::transform()
    {
    if (space_ == RealSpace)
        {
        fftw_execute(r2c_plan_);
        space_ = ReciprocalSpace;
        }
    else
        {
        // execute inverse FFT and renormalize by N (FFTW does not)
        fftw_execute(c2r_plan_);
        const auto size = mesh_.shape();
        std::transform(data_,data_+size,data_,[&](auto x){return x/size;});
        space_ = RealSpace;
        }
    }

const Mesh& FourierTransform::getMesh() const
    {
    return mesh_;
    }

const FourierTransform::Wavevectors& FourierTransform::getWavevectors() const
    {
    return kmesh_;
    }

FourierTransform::Wavevectors::Wavevectors(double L, int N)
    {
    step_ = (2.*M_PI)/L;
    shape_ = N/2+1;
    }

double FourierTransform::Wavevectors::operator()(int i) const
    {
    return static_cast<double>(i)*step_;
    }

int FourierTransform::Wavevectors::shape() const
    {
    return shape_;
    }

}
