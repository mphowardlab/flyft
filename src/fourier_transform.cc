#include "flyft/fourier_transform.h"

namespace flyft
{
FourierTransform::FourierTransform(int N)
    : N_(N), step_(step)
    {
    space_ = RealSpace;

    // size of the data buffer required by fftw for r2c and c2r transforms
    data_ = std::shared_ptr<double>(fftw_alloc_real(2*(N_/2+1)),
                                    [](double* p)
                                        {
                                        if(p) fftw_free(p);
                                        }
                                    );

    r2c_plan_ = fftw_plan_dft_r2c_1d(N_,
                                     data_.get(),
                                     reinterpret_cast<Complex*>(data_.get()),
                                     FFTW_ESTIMATE);

    c2r_plan_ = fftw_plan_dft_c2r_1d(N_,
                                     reinterpret_cast<Complex*>(data_.get()),
                                     data_.get(),
                                     FFTW_ESTIMATE);
    }

FourierTransform::~FourierTransform()
    {
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
	return static_cast<const double*>(data_.get());
	}

void FourierTransform::setRealData(const double* data)
    {
    memcpy(data_.get(), data, getRealSize()*sizeof(double));
    space_ = RealSpace;
    }

int FourierTransform::getReciprocalSize() const
    {
    return (N_/2+1);
    }

const FourierTransform::Complex* FourierTransform::getReciprocalData() const
	{
	if (space_ != ReciprocalSpace)
	    {
	    // raise error, buffer not valid
	    }
	return reinterpret_cast<const Complex*>(data_.get());
	}

void FourierTransform::setReciprocalData(const FourierTransform::Complex* data)
    {
    memcpy(data_.get(), data, getReciprocalSize()*sizeof(Complex));
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
        fftw_execute(c2r_plan_);

        // renormalize inverse FFT by factor of N
        auto data = data_.get();
        for (int i=0; i < N_; ++i)
            {
            data[i] /= N_;
            }

        space_ = RealSpace;
        }
    }
}
