#ifndef FLYFT_LINEAR_POTENTIAL_H_
#define FLYFT_LINEAR_POTENTIAL_H_

#include "flyft/external_potential.h"
#include "flyft/field.h"
#include "flyft/state.h"
#include "flyft/type_map.h"

#include <memory>
#include <string>

namespace flyft
{

class LinearPotential : public ExternalPotential
    {
    public:
        LinearPotential();

        void potential(std::shared_ptr<Field> V,
                       const std::string& type,
                       std::shared_ptr<State> state) override;

        TypeMap<double>& getXs();
        const TypeMap<double>& getXs() const;

        TypeMap<double>& getYs();
        const TypeMap<double>& getYs() const;

        TypeMap<double>& getSlopes();
        const TypeMap<double>& getSlopes() const;

    protected:
        TypeMap<double> xs_;
        TypeMap<double> ys_;
        TypeMap<double> slopes_;
    };

}

#endif // FLYFT_LINEAR_POTENTIAL_H_
