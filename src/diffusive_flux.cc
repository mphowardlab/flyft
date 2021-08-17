#include "flyft/diffusive_flux.h"

namespace flyft
{
TypeMap<double>& DiffusiveFlux::getDiffusivities()
    {
    return diffusivities_;
    }

const TypeMap<double>& DiffusiveFlux::getDiffusivities() const
    {
    return diffusivities_;
    }

}
