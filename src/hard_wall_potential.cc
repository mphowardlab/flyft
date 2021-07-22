#include "flyft/hard_wall_potential.h"

#include <cmath>

namespace flyft
{

void HardWallPotential::potential(std::shared_ptr<Field> V, const std::string& type, std::shared_ptr<State> state)
    {
    // edge where sphere contacts the wall
    const double R = 0.5*diameters_.at(type);
    const double edge = origin_->evaluate(state) + normal_*R;
    const auto normal = normal_;

    const auto mesh = *state->getMesh();
    auto data = V->first();

    #ifdef FLYFT_OPENMP
    #pragma omp parallel for schedule(static) default(none) firstprivate(mesh,edge,normal) shared(data)
    #endif
    for (int idx=0; idx != mesh.shape(); ++idx)
        {
        const auto x = mesh.coordinate(idx);
        data[idx] = (normal*(x-edge) < 0) ? std::numeric_limits<double>::infinity() : 0.0;
        }
    }

const TypeMap<double>& HardWallPotential::getDiameters()
    {
    return diameters_;
    }

double HardWallPotential::getDiameter(const std::string& type) const
    {
    return diameters_.at(type);
    }

void HardWallPotential::setDiameters(const TypeMap<double>& diameters)
    {
    diameters_ = TypeMap<double>(diameters);
    }

void HardWallPotential::setDiameter(const std::string& type, double diameter)
    {
    if (diameter >= 0.)
        {
        diameters_[type] = diameter;
        }
    else
        {
        // error: invalid diameter
        }
    }

}
