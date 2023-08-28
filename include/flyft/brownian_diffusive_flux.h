#ifndef FLYFT_BROWNIAN_DIFFUSIVE_FLUX_H_
#define FLYFT_BROWNIAN_DIFFUSIVE_FLUX_H_

#include "flyft/flux.h"
#include "flyft/grand_potential.h"
#include "flyft/state.h"

#include <memory>

namespace flyft
{

class BrownianDiffusiveFlux : public Flux
    {
    public: 
    
        TypeMap<double>& getDiffusivities();
        const TypeMap<double>& getDiffusivities() const;
        
    protected:
        void compute(std::shared_ptr<GrandPotential> grand, std::shared_ptr<State> state) override;
        
        int determineBufferShape(std::shared_ptr<State> state, const std::string& type) override; 
    
    private:
        TypeMap<double> diffusivities_;
    };

}

#endif // FLYFT_BROWNIAN_DIFFUSIVE_FLUX_H_
