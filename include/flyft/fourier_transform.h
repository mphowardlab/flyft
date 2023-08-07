#ifndef FLYFT_FOURIER_TRANSFORM_H_
#define FLYFT_FOURIER_TRANSFORM_H_

#include "flyft/data_layout.h"
#include "flyft/data_view.h"
#include "flyft/cartesian_mesh.h"

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

        using RealView = DataView<double>;
        using ConstantRealView = DataView<const double>;
        using ReciprocalView = DataView<std::complex<double>>;
        using ConstantReciprocalView = DataView<const std::complex<double>>;

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

        RealView view_real() const;
        ConstantRealView const_view_real() const;
        void setRealData(const RealView& data);
        void setRealData(const ConstantRealView& data);

        ReciprocalView view_reciprocal() const;
        ConstantReciprocalView const_view_reciprocal() const;
        void setReciprocalData(const ReciprocalView& data);
        void setReciprocalData(const ConstantReciprocalView& data);

        Space getActiveSpace() const;

        double getL() const;
        int getN() const;
        const Wavevectors& getWavevectors() const;

    private:
        double L_;
        int N_;
        Wavevectors kmesh_;
        Space space_;
        double* data_;
        fftw_plan r2c_plan_;
        fftw_plan c2r_plan_;
    };

}

#endif // FLYFT_FOURIER_TRANSFORM_H_
