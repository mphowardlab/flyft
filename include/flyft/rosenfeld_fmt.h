#ifndef FLYFT_ROSENFELD_FMT_H_
#define FLYFT_ROSENFELD_FMT_H_

#include "flyft/field.h"
#include "flyft/fourier_transform.h"
#include "flyft/functional.h"
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

        void compute(std::shared_ptr<State> state) override;

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

        std::unique_ptr<ComplexField> n0k_;
        std::unique_ptr<ComplexField> n1k_;
        std::unique_ptr<ComplexField> n2k_;
        std::unique_ptr<ComplexField> n3k_;
        std::unique_ptr<ComplexField> nv1k_;
        std::unique_ptr<ComplexField> nv2k_;

        std::unique_ptr<ComplexField> dphi_dn0k_;
        std::unique_ptr<ComplexField> dphi_dn1k_;
        std::unique_ptr<ComplexField> dphi_dn2k_;
        std::unique_ptr<ComplexField> dphi_dn3k_;
        std::unique_ptr<ComplexField> dphi_dnv1k_;
        std::unique_ptr<ComplexField> dphi_dnv2k_;

        std::unique_ptr<ComplexField> derivativek_;

        TypeMap<std::unique_ptr<ComplexField>> w0k_;
        TypeMap<std::unique_ptr<ComplexField>> w3k_;

        bool setup(std::shared_ptr<State> state) override;

        void setupField(std::shared_ptr<Field>& field);
        void setupComplexField(std::unique_ptr<ComplexField>& kfield);

        // replace this by a template evaluator for flexibility (requires templating whole class though)
        void computeWeights(const std::complex<double>& w0,
                            std::complex<double>& w1,
                            std::complex<double>& w2,
                            const std::complex<double>& w3,
                            std::complex<double>& wv1,
                            std::complex<double>& wv2,
                            double k,
                            double R) const;
    };

}

#endif // FLYFT_ROSENFELD_FMT_H_
