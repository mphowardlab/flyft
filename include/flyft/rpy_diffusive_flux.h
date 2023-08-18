#ifndef FLYFT_RPY_DIFFUSIVE_FLUX_H_
#define FLYFT_RPY_DIFFUSIVE_FLUX_H_

#include "flyft/diffusive_flux.h"
#include "flyft/grand_potential.h"
#include "flyft/state.h"
#include "flyft/type_map.h"

#include <memory>
#include <exception>
#include <bits/stdc++.h>

namespace flyft
{

class RPYDiffusiveFlux : public DiffusiveFlux
    {
    public:
    
        TypeMap<double>& getDiameters();
        const TypeMap<double>& getDiameters() const;
        void compute(std::shared_ptr<GrandPotential> grand, std::shared_ptr<State> state) override;
        int determineBufferShape(std::shared_ptr<State> state, const std::string& type) override;
    
    private:
        TypeMap<double> diameters_;
    };
}

#endif // FLYFT_RPY_DIFFUSIVE_FLUX_H_
