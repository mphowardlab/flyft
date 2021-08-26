#include "flyft/harmonic_wall_potential.h"

namespace flyft
{

HarmonicWallPotential::HarmonicWallPotential(double origin, double normal)
    : HarmonicWallPotential(std::make_shared<ConstantDoubleParameter>(origin),normal)
    {
    }

HarmonicWallPotential::HarmonicWallPotential(std::shared_ptr<DoubleParameter> origin, double normal)
    : WallPotential(origin,normal)
    {
    compute_depends_.add(&spring_constants_);
    compute_depends_.add(&shifts_);
    }

void HarmonicWallPotential::potential(std::shared_ptr<Field> V, const std::string& type, std::shared_ptr<State> state)
    {
    // get potential parameters for this type
    const auto k = spring_constants_(type);
    const auto shift = shifts_(type);
    const double x0 = origin_->evaluate(state) + shift;
    const auto normal = normal_->evaluate(state);

    const auto mesh = *state->getMesh()->local();
    auto data = V->view();
    #ifdef FLYFT_OPENMP
    #pragma omp parallel for schedule(static) default(none) firstprivate(k,x0,normal,mesh) shared(data)
    #endif
    for (int idx=0; idx < mesh.shape(); ++idx)
        {
        const auto x = mesh.coordinate(idx);
        const double dx = normal*(x-x0);
        // potential acts only if dx < 0 (x is "inside" the wall)
        data(idx) = (dx < 0) ? 0.5*k*dx*dx : 0.0;
        }
    }

TypeMap<double>& HarmonicWallPotential::getSpringConstants()
    {
    return spring_constants_;
    }

const TypeMap<double>& HarmonicWallPotential::getSpringConstants() const
    {
    return spring_constants_;
    }

TypeMap<double>& HarmonicWallPotential::getShifts()
    {
    return shifts_;
    }

const TypeMap<double>& HarmonicWallPotential::getShifts() const
    {
    return shifts_;
    }

}
