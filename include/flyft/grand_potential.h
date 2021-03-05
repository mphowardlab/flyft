#ifndef FLYFT_GRAND_POTENTIAL_H_
#define FLYFT_GRAND_POTENTIAL_H_

#include "flyft/composite_functional.h"
#include "flyft/external_potential.h"
#include "flyft/free_energy_functional.h"
#include "flyft/ideal_gas_functional.h"

#include <memory>
#include <string>

namespace flyft
{

class GrandPotential : public FreeEnergyFunctional
    {
    public:
        GrandPotential();

        void compute(std::shared_ptr<State> state) override;

        std::shared_ptr<IdealGasFunctional> getIdealGasFunctional();
        std::shared_ptr<CompositeFunctional> getIntrinsicFunctional();
        std::shared_ptr<CompositeFunctional> getExcessIntrinsicFunctional();
        std::shared_ptr<CompositeFunctional> getExternalPotential();

        void addExcessFunctional(std::shared_ptr<FreeEnergyFunctional> functional);
        void removeExcessFunctional(std::shared_ptr<FreeEnergyFunctional> functional);

        void addExternalPotential(std::shared_ptr<ExternalPotential> functional);
        void removeExternalPotential(std::shared_ptr<ExternalPotential> functional);

        enum class Constraint
            {
            N,
            mu
            };
        const TypeMap<double>& getConstraints();
        double getConstraint(const std::string& type) const;
        void setConstraints(const TypeMap<double>& constraints);
        void setConstraint(const std::string& type, double value);

        const TypeMap<Constraint>& getConstraintTypes();
        Constraint getConstraintType(const std::string& type) const;
        void setConstraintTypes(const TypeMap<Constraint>& constraint_types);
        void setConstraintType(const std::string& type, Constraint ctype);

    private:
        std::shared_ptr<IdealGasFunctional> ideal_;
        std::shared_ptr<CompositeFunctional> intrinsic_;
        std::shared_ptr<CompositeFunctional> excess_;
        std::shared_ptr<CompositeFunctional> external_;
        TypeMap<double> constraints_;
        TypeMap<Constraint> constraint_types_;
    };

}

#endif // FLYFT_GRAND_POTENTIAL_H_
