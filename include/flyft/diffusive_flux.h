#ifndef FLYFT_DIFFUSIVE_FLUX_H_
#define FLYFT_DIFFUSIVE_FLUX_H_

#include "flyft/flux.h"
#include "flyft/type_map.h"

#include <string>

namespace flyft
{

class DiffusiveFlux: public Flux
    {
    public:
        const TypeMap<double>& getDiffusivities();
        double getDiffusivity(const std::string& type) const;
        void setDiffusivities(const TypeMap<double>& diffusivities);
        void setDiffusivity(const std::string& type, double diffusivity);

    protected:
        TypeMap<double> diffusivities_;
    };

}

#endif // FLYFT_DIFFUSIVE_FLUX_H_
