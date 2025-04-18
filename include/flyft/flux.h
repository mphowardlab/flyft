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
    virtual void requestFluxBuffer(const std::string& type, int buffer_request);

    virtual int determineBufferShape(std::shared_ptr<State> state, const std::string& type);

    protected:
    TypeMap<std::shared_ptr<Field>> fluxes_;
    TypeMap<int> buffer_requests_;

    virtual void setup(std::shared_ptr<GrandPotential> grand, std::shared_ptr<State> state);

    virtual bool validateConstraints(std::shared_ptr<GrandPotential> grand,
                                     std::shared_ptr<State> state) const;
    };

    } // namespace flyft

#endif // FLYFT_FLUX_H_
