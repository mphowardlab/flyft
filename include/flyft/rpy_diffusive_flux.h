#ifndef FLYFT_RPY_DIFFUSIVE_FLUX_H_
#define FLYFT_RPY_DIFFUSIVE_FLUX_H_

#include "flyft/flux.h"
#include "flyft/grand_potential.h"
#include "flyft/grid_interpolator.h"
#include "flyft/state.h"
#include "flyft/type_map.h"

#include <memory>

namespace flyft
{

class RPYDiffusiveFlux : public Flux
    {
    public:
        RPYDiffusiveFlux();
        
        TypeMap<double>& getDiameters();         
        const TypeMap<double>& getDiameters() const;
        
        double getViscosity() const;
        void setViscosity(double viscosity);
        
    protected:
        void compute(std::shared_ptr<GrandPotential> grand, std::shared_ptr<State> state) override;
        int determineBufferShape(std::shared_ptr<State> state, const std::string&) override;
    
    private:
        std::shared_ptr<GridInterpolator> mobility_;
        TypeMap<double> diameters_;
        double viscosity_;
    };
}

#endif // FLYFT_RPY_DIFFUSIVE_FLUX_H_
