#include "flyft/fourier_transform.h"

#include <algorithm>
#ifdef FLYFT_OPENMP
#include <omp.h>
#endif

namespace flyft
{
FourierTransform::FourierTransform(int N)
    : N_(N), space_(RealSpace)
    {
    #ifdef FLYFT_OPENMP
    // use all available OpenMP threads
    fftw_init_threads();
    fftw_plan_with_nthreads(omp_get_max_threads());
    #endif

    // this is the doc'd size of "real" memory required for the r2c / c2r transform
    data_ = fftw_alloc_real(2*(N_/2+1));

    r2c_plan_ = fftw_plan_dft_r2c_1d(N_,
                                     data_,
                                     reinterpret_cast<fftw_complex*>(data_),
                                     FFTW_ESTIMATE);

    c2r_plan_ = fftw_plan_dft_c2r_1d(N_,
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

int FourierTransform::getRealSize() const
    {
    return N_;
    }

const double* FourierTransform::getRealData() const
    {
    if (space_ != RealSpace)
        {
        // raise error, buffer not valid
        }
    return static_cast<const double*>(data_);
    }

void FourierTransform::setRealData(const double* data)
    {
    const auto size = getRealSize();
    std::copy(data,data+size,data_);
    space_ = RealSpace;
    }

int FourierTransform::getReciprocalSize() const
    {
    return (N_/2+1);
    }

const std::complex<double>* FourierTransform::getReciprocalData() const
    {
    if (space_ != ReciprocalSpace)
        {
        // raise error, buffer not valid
        }
    return reinterpret_cast<const std::complex<double>*>(data_);
    }

void FourierTransform::setReciprocalData(const std::complex<double>* data)
    {
    auto p = reinterpret_cast<const double*>(data);
    const auto size = 2*getReciprocalSize();
    std::copy(p,p+size,data_);
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
        std::transform(data_,data_+N_,data_,[&](auto x){return x/N_;});
        space_ = RealSpace;
        }
    }
}
