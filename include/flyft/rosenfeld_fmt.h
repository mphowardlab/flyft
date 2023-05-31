#ifndef FLYFT_ROSENFELD_FMT_H_
#define FLYFT_ROSENFELD_FMT_H_

#include "flyft/field.h"
#include "flyft/fourier_transform.h"
#include "flyft/functional.h"
#include "flyft/mesh.h"
#include "flyft/state.h"
#include "flyft/type_map.h"

#include <complex>
#include <memory>
#include <string>
#include <vector>

namespace flyft
{

class RosenfeldFMT : public Functional
    {
    public:
        RosenfeldFMT();

        TypeMap<double>& getDiameters();
        const TypeMap<double>& getDiameters() const;

        int determineBufferShape(std::shared_ptr<State> state, const std::string& type) override;

    protected:
        TypeMap<double> diameters_;
        std::unique_ptr<FourierTransform> ft_;
        int buffer_shape_;

        std::shared_ptr<Field> n0_;
        std::shared_ptr<Field> n1_;
        std::shared_ptr<Field> n2_;
        std::shared_ptr<Field> n3_;
        std::shared_ptr<Field> nv1_;
        std::shared_ptr<Field> nv2_;

        std::shared_ptr<Field> phi_;
        std::shared_ptr<Field> dphi_dn0_;
        std::shared_ptr<Field> dphi_dn1_;
        std::shared_ptr<Field> dphi_dn2_;
        std::shared_ptr<Field> dphi_dn3_;
        std::shared_ptr<Field> dphi_dnv1_;
        std::shared_ptr<Field> dphi_dnv2_;

        std::unique_ptr<ComplexField> dphi_dn0k_;
        std::unique_ptr<ComplexField> dphi_dn1k_;
        std::unique_ptr<ComplexField> dphi_dn2k_;
        std::unique_ptr<ComplexField> dphi_dn3k_;
        std::unique_ptr<ComplexField> dphi_dnv1k_;
        std::unique_ptr<ComplexField> dphi_dnv2k_;

        std::unique_ptr<ComplexField> derivativek_;

        bool setup(std::shared_ptr<State> state, bool compute_value) override;
        void _compute(std::shared_ptr<State> state, bool compute_value) override;

        void computeCartesianWeightedDensities(std::shared_ptr<State> state);
        void computeSphericalWeightedDensities(std::shared_ptr<State> state);
        
        std::map<std::string,std::shared_ptr<Field>> tmp_field_;
        std::map<std::string,std::unique_ptr<ComplexField>> tmp_complex_field_;

        void computePhiAndDerivatives(int idx,
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
                                      bool compute_value) const;
        
        void computeCartesianDerivative(std::shared_ptr<State> state);
        void computeSphericalDerivative(std::shared_ptr<State> state);
        
        virtual void computePrefactorFunctions(double& f1,
                                               double& f2,
                                               double& f4,
                                               double& df1dn3,
                                               double& df2dn3,
                                               double& df4dn3,
                                               double n3) const;
        void computeWeights(std::complex<double>& w2,
                            std::complex<double>& w3,
                            std::complex<double>& wv2,
                            double k,
                            double R) const;
        template<typename T>
        void computeProportionalByWeight(T& w0, T& w1, T& wv1, const T& w2, const T& wv2, const double R) const;

        void setupField(std::shared_ptr<Field>& field);
        void setupComplexField(std::unique_ptr<ComplexField>& kfield);

        enum struct ConvolutionType
            {
            cartesian, spherical
            };
        ConvolutionType getConvolutionType(std::shared_ptr<const Mesh> mesh) const;

        std::shared_ptr<Field> tmp_r_field_;
        void fourierTransformFieldSpherical(const Field::ConstantView& input,
                                            const Mesh* mesh) const;
    };

template<typename T>
void RosenfeldFMT::computeProportionalByWeight(T& w0, T& w1, T& wv1, const T& w2, const T& wv2, const double R) const
    {
    const double factor = 1./(4.*M_PI*R);
    w1 = w2 * factor;
    w0 = w1 / R;
    wv1 = wv2 * factor;
    }

}

#endif // FLYFT_ROSENFELD_FMT_H_
