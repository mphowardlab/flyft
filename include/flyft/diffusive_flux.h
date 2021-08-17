#ifndef FLYFT_DIFFUSIVE_FLUX_H_
#define FLYFT_DIFFUSIVE_FLUX_H_

#include "flyft/flux.h"
#include "flyft/type_map.h"

namespace flyft
{

class DiffusiveFlux: public Flux
    {
    public:
        TypeMap<double>& getDiffusivities();
        const TypeMap<double>& getDiffusivities() const;

    protected:
        TypeMap<double> diffusivities_;
    };

}

#endif // FLYFT_DIFFUSIVE_FLUX_H_
