#include "flyft/diffusive_flux.h"

namespace flyft
{

const TypeMap<double>& DiffusiveFlux::getDiffusivities()
    {
    return diffusivities_;
    }

double DiffusiveFlux::getDiffusivity(const std::string& type) const
    {
    return diffusivities_.at(type);
    }

void DiffusiveFlux::setDiffusivities(const TypeMap<double>& diffusivities)
    {
    diffusivities_ = TypeMap<double>(diffusivities);
    }

void DiffusiveFlux::setDiffusivity(const std::string& type, double diffusivity)
    {
    diffusivities_[type] = diffusivity;
    }

}
