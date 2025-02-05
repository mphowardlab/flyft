#ifndef FLYFT_COMPOSITE_EXTERNAL_POTENTIAL_H_
#define FLYFT_COMPOSITE_EXTERNAL_POTENTIAL_H_

#include "flyft/composite_mixin.h"
#include "flyft/external_potential.h"
#include "flyft/field.h"
#include "flyft/state.h"

#include <memory>
#include <string>

namespace flyft
    {

class CompositeExternalPotential : public ExternalPotential,
                                   public CompositeMixin<ExternalPotential>
    {
    public:
    CompositeExternalPotential();

    void requestDerivativeBuffer(const std::string& type, int buffer_request) override;

    bool addObject(std::shared_ptr<ExternalPotential> object);
    bool removeObject(std::shared_ptr<ExternalPotential> object);
    void clearObjects();

    protected:
    bool setup(std::shared_ptr<State> state, bool compute_value) override;
    void computePotentials(std::shared_ptr<State> state) override;
    };

    } // namespace flyft

#endif // FLYFT_COMPOSITE_EXTERNAL_POTENTIAL_H_
