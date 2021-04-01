#ifndef FLYFT_COMPOSITE_EXTERNAL_POTENTIAL_H_
#define FLYFT_COMPOSITE_EXTERNAL_POTENTIAL_H_

#include "flyft/external_potential.h"
#include "flyft/state.h"

#include <memory>
#include <vector>

namespace flyft
{

class CompositeExternalPotential : public ExternalPotential
    {
    public:
        CompositeExternalPotential();
        CompositeExternalPotential(std::shared_ptr<ExternalPotential> potential);
        CompositeExternalPotential(const std::vector<std::shared_ptr<ExternalPotential>>& potentials);

        void addPotential(std::shared_ptr<ExternalPotential> potential);
        void removePotential(std::shared_ptr<ExternalPotential> potential);
        void clearPotentials();
        const std::vector<std::shared_ptr<ExternalPotential>>& getPotentials();

        void potential(std::shared_ptr<Field> V, const std::string& type, std::shared_ptr<State> state) override;

    protected:
        void allocate(std::shared_ptr<State> state) override;

    private:
        std::vector<std::shared_ptr<ExternalPotential>> potentials_;
        std::shared_ptr<Field> Vtmp_;
    };

}

#endif // FLYFT_COMPOSITE_EXTERNAL_POTENTIAL_H_
