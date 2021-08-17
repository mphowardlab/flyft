#include "flyft/hard_wall_potential.h"

#include <cmath>

namespace flyft
{

void HardWallPotential::potential(std::shared_ptr<Field> V, const std::string& type, std::shared_ptr<State> state)
    {
    // edge where sphere contacts the wall
    const double R = 0.5*diameters_(type);
    const double edge = origin_->evaluate(state) + normal_*R;
    const auto normal = normal_;

    const auto mesh = *state->getMesh()->local();
    auto data = V->view();
    #ifdef FLYFT_OPENMP
    #pragma omp parallel for schedule(static) default(none) firstprivate(mesh,edge,normal) shared(data)
    #endif
    for (int idx=0; idx < mesh.shape(); ++idx)
        {
        const auto x = mesh.coordinate(idx);
        data(idx) = (normal*(x-edge) < 0) ? std::numeric_limits<double>::infinity() : 0.0;
        }
    }

TypeMap<double>& HardWallPotential::getDiameters()
    {
    return diameters_;
    }

const TypeMap<double>& HardWallPotential::getDiameters() const
    {
    return diameters_;
    }

}
