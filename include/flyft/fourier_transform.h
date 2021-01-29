#ifndef FLYFT_FOURIER_TRANSFORM_H_
#define FLYFT_FOURIER_TRANSFORM_H_

#include <fftw3.h>
#include <memory>

namespace flyft
{

class FourierTransform
    {
    public:
        FourierTransform() = delete;
        FourierTransform(int N, double step);

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

        std::shared_ptr<double> getData();
        void setData(const void* data, Space space);
        Space getSpace() const;

        void transform();

    private:
        int N_;
        double step_;

        Space space_;
        std::shared_ptr<double> data_;
        fftw_plan r2c_plan_;
        fftw_plan c2r_plan_;
    };

}

#endif // FLYFT_FOURIER_TRANSFORM_H_
