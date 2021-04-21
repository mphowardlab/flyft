#ifndef FLYFT_FOURIER_TRANSFORM_H_
#define FLYFT_FOURIER_TRANSFORM_H_

// need to include <complex> before <fftw3.h>
#include <complex>
#include <fftw3.h>

#include <memory>

namespace flyft
{

class FourierTransform
    {
    public:
        FourierTransform() = delete;
        FourierTransform(int N);
        ~FourierTransform();

        // noncopyable / nonmovable
        FourierTransform(const FourierTransform&) = delete;
        FourierTransform(FourierTransform&&) = delete;
        FourierTransform& operator=(const FourierTransform&) = delete;
        FourierTransform& operator=(FourierTransform&&) = delete;

        enum Space
            {
            RealSpace,
            ReciprocalSpace
            };

        void transform();

        int getRealSize() const;
        const double* getRealData() const;
        void setRealData(const double* data);

        int getReciprocalSize() const;
        const std::complex<double>* getReciprocalData() const;
        void setReciprocalData(const std::complex<double>* data);

        Space getActiveSpace() const;

    private:
        int N_;
        Space space_;
        double* data_;
        fftw_plan r2c_plan_;
        fftw_plan c2r_plan_;
    };

}

#endif // FLYFT_FOURIER_TRANSFORM_H_
