#include "flyft/fourier_transform.h"

namespace flyft
{
FourierTransform::FourierTransform(int N, double step)
    : N_(N), step_(step)
    {
    space_ = RealSpace;
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

// FourierTransform::coordinate(int i) const
//     {
//     if ((N_ % 2 == 0 && i < N_/2) || (N_ % 2 == 1 && i <= N_/2))
//         {
//         return i/(N_*step_);
//         }
//     else
//         {
//         return -(N_-i)/(N_*step);
//         }
//     }

std::shared_ptr<double> FourierTransform::getData()
	{
	return data_;
	}

void FourierTransform::setData(const void* data, Space space)
    {
    size_t count = (space == RealSpace) ? N_*sizeof(double) : (N_/2+1)*sizeof(Complex);
    memcpy(data_.get(), data, count);
    space_ = space;
    }

FourierTransform::Space FourierTransform::getSpace() const
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
