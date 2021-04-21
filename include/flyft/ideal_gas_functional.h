#ifndef IDEAL_GAS_FUNCTIONAL_H_
#define IDEAL_GAS_FUNCTIONAL_H_

#include "flyft/functional.h"
#include "flyft/state.h"
#include "flyft/type_map.h"

#include <memory>

namespace flyft
{

class IdealGasFunctional : public Functional
    {
    public:
        void compute(std::shared_ptr<State> state) override;

        const TypeMap<double>& getVolumes();
        double getVolume(const std::string& type) const;
        void setVolumes(const TypeMap<double>& volumes);
        void setVolume(const std::string& type, double volume);

    private:
        TypeMap<double> volumes_;
    };

}

#endif // IDEAL_GAS_FUNCTIONAL_H_
