#ifndef FLYFT_FLUX_H_
#define FLYFT_FLUX_H_

#include "flyft/field.h"
#include "flyft/grand_potential.h"
#include "flyft/state.h"
#include "flyft/type_map.h"

#include <memory>
#include <string>

namespace flyft
{

class Flux
    {
    public:
        Flux();
        virtual ~Flux();

        // noncopyable / nonmovable
        Flux(const Flux&) = delete;
        Flux(Flux&&) = delete;
        Flux& operator=(const Flux&) = delete;
        Flux& operator=(Flux&&) = delete;

        virtual void compute(std::shared_ptr<GrandPotential> grand, std::shared_ptr<State> state) = 0;

        const TypeMap<std::shared_ptr<Field>>& getFluxes();
        std::shared_ptr<Field> getFlux(const std::string& type);
        std::shared_ptr<const Field> getFlux(const std::string& type) const;

    protected:
        TypeMap<std::shared_ptr<Field>> fluxes_;

        virtual void allocate(std::shared_ptr<State> state);

        virtual bool validateConstraints(std::shared_ptr<GrandPotential> grand, std::shared_ptr<State> state) const;
    };

}

#endif // FLYFT_FLUX_H_
