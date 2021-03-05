#ifndef FLYFT_ROSENFELD_FMT_H_
#define FLYFT_ROSENFELD_FMT_H_

#include "flyft/field.h"
#include "flyft/fourier_transform.h"
#include "flyft/free_energy_functional.h"
#include "flyft/state.h"
#include "flyft/type_map.h"

#include <complex>
#include <memory>
#include <vector>

namespace flyft
{

class RosenfeldFMT : public FreeEnergyFunctional
    {
    public:
        void compute(std::shared_ptr<State> state) override;

        const TypeMap<double>& getDiameters();
        double getDiameter(const std::string& type) const;
        void setDiameters(const TypeMap<double>& diameters);
        void setDiameter(const std::string& type, double diameter);

    protected:
        TypeMap<double> diameters_;
        std::unique_ptr<FourierTransform> ft_;

        std::unique_ptr<Field> n0_;
        std::unique_ptr<Field> n1_;
        std::unique_ptr<Field> n2_;
        std::unique_ptr<Field> n3_;
        std::unique_ptr<Field> nv1_;
        std::unique_ptr<Field> nv2_;

        std::unique_ptr<Field> phi_;
        std::unique_ptr<Field> dphi_dn0_;
        std::unique_ptr<Field> dphi_dn1_;
        std::unique_ptr<Field> dphi_dn2_;
        std::unique_ptr<Field> dphi_dn3_;
        std::unique_ptr<Field> dphi_dnv1_;
        std::unique_ptr<Field> dphi_dnv2_;

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

        void allocate(std::shared_ptr<State> state) override;

        void setupField(std::unique_ptr<Field>& field, int shape);
        void setupComplexField(std::unique_ptr<ComplexField>& kfield, int kshape);
        void setupFourierFields(std::unique_ptr<Field>& field,
                                std::unique_ptr<ComplexField>& kfield,
                                int shape,
                                int kshape);

        // replace this by a template evaluator for flexibility (requires templating whole class though)
        void computeWeights(std::complex<double>& w0,
                            std::complex<double>& w1,
                            std::complex<double>& w2,
                            std::complex<double>& w3,
                            std::complex<double>& wv1,
                            std::complex<double>& wv2,
                            double k,
                            double R) const;
    };

}

#endif // FLYFT_ROSENFELD_FMT_H_
