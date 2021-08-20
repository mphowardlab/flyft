#ifndef FLYFT_FOURIER_TRANSFORM_H_
#define FLYFT_FOURIER_TRANSFORM_H_

#include "flyft/mesh.h"

// need to include <complex> before <fftw3.h>
#include <complex>
#include <fftw3.h>

#include <memory>

namespace flyft
{

class FourierTransform
    {
    public:
        class Wavevectors
            {
            public:
                Wavevectors() = delete;
                Wavevectors(double L, int N);
                double operator()(int i) const;
                int shape() const;

            private:
                double step_;   //!< Spacing between mesh points
                int shape_;     //!< Shape of underlying mesh
            };

        FourierTransform() = delete;
        FourierTransform(double L, int shape);
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

        const double* getRealData() const;
        void setRealData(const double* data);

        const std::complex<double>* getReciprocalData() const;
        void setReciprocalData(const std::complex<double>* data);

        Space getActiveSpace() const;

        const Mesh& getMesh() const;
        const Wavevectors& getWavevectors() const;

    private:
        Mesh mesh_;
        Wavevectors kmesh_;
        Space space_;
        double* data_;
        fftw_plan r2c_plan_;
        fftw_plan c2r_plan_;
    };

}

#endif // FLYFT_FOURIER_TRANSFORM_H_
