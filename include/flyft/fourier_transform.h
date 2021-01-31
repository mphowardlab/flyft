#ifndef FLYFT_FOURIER_TRANSFORM_H_
#define FLYFT_FOURIER_TRANSFORM_H_

// use native C complex type to get complex math
#include <complex.h>
#include <fftw3.h>
#include <memory>

namespace flyft
{

class FourierTransform
    {
    public:
        FourierTransform() = delete;
        FourierTransform(int N);

        // noncopyable / nonmovable
        FourierTransform(const FourierTransform&) = delete;
        FourierTransform(FourierTransform&&) = delete;
        FourierTransform& operator=(const FourierTransform&) = delete;
        FourierTransform& operator=(FourierTransform&&) = delete;

        ~FourierTransform();

        typedef fftw_complex Complex;
        enum Space
            {
            RealSpace,
            ReciprocalSpace
            };

        int getRealSize() const;
        const double* getRealData() const;
        void setRealData(const double* data);

        int getReciprocalSize() const;
        const Complex* getReciprocalData() const;
        void setReciprocalData(const Complex* data);

        Space getActiveSpace() const;

        void transform();

    private:
        int N_;
        Space space_;
        std::shared_ptr<double> data_;
        fftw_plan r2c_plan_;
        fftw_plan c2r_plan_;
    };

}

#endif // FLYFT_FOURIER_TRANSFORM_H_
