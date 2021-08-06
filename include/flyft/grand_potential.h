#ifndef FLYFT_GRAND_POTENTIAL_H_
#define FLYFT_GRAND_POTENTIAL_H_

#include "flyft/external_potential.h"
#include "flyft/functional.h"
#include "flyft/ideal_gas_functional.h"
#include "flyft/state.h"
#include "flyft/type_map.h"

#include <memory>
#include <string>

namespace flyft
{

class GrandPotential : public Functional
    {
    public:
        GrandPotential();

        void compute(std::shared_ptr<State> state) override;
        void requestDerivativeBuffer(const std::string& type, int buffer_request) override;
        int determineBufferShape(std::shared_ptr<State> state, const std::string& type) override;

        std::shared_ptr<IdealGasFunctional> getIdealGasFunctional();
        std::shared_ptr<const IdealGasFunctional> getIdealGasFunctional() const;
        void setIdealGasFunctional(std::shared_ptr<IdealGasFunctional> ideal);

        std::shared_ptr<Functional> getExcessFunctional();
        std::shared_ptr<const Functional> getExcessFunctional() const;
        void setExcessFunctional(std::shared_ptr<Functional> excess);

        std::shared_ptr<ExternalPotential> getExternalPotential();
        std::shared_ptr<const ExternalPotential> getExternalPotential() const;
        void setExternalPotential(std::shared_ptr<ExternalPotential> external);

        enum class Constraint
            {
            N,
            mu
            };
        const TypeMap<double>& getConstraints();
        double getConstraint(const std::string& type) const;
        void setConstraints(const TypeMap<double>& constraints);
        void setConstraint(const std::string& type, double constraint);

        const TypeMap<Constraint>& getConstraintTypes();
        Constraint getConstraintType(const std::string& type) const;
        void setConstraintTypes(const TypeMap<Constraint>& constraint_types);
        void setConstraintType(const std::string& type, Constraint constraint_type);

    private:
        std::shared_ptr<IdealGasFunctional> ideal_;
        std::shared_ptr<Functional> excess_;
        std::shared_ptr<ExternalPotential> external_;
        TypeMap<double> constraints_;
        TypeMap<Constraint> constraint_types_;
    };

}

#endif // FLYFT_GRAND_POTENTIAL_H_
