#include "flyft/harmonic_wall_potential.h"

namespace flyft
{

void HarmonicWallPotential::potential(std::shared_ptr<Field> V, const std::string& type, std::shared_ptr<State> state)
    {
    // get potential parameters for this type
    const auto k = spring_constants_.at(type);
    const auto shift = shifts_.at(type);
    const double x0 = origin_->evaluate(state) + shift;
    const auto normal = normal_;

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

const TypeMap<double>& HarmonicWallPotential::getSpringConstants()
    {
    return spring_constants_;
    }

double HarmonicWallPotential::getSpringConstant(const std::string& type) const
    {
    return spring_constants_.at(type);
    }

void HarmonicWallPotential::setSpringConstants(const TypeMap<double>& spring_constants)
    {
    spring_constants_ = TypeMap<double>(spring_constants);
    }

void HarmonicWallPotential::setSpringConstant(const std::string& type, double spring_constant)
    {
    spring_constants_[type] = spring_constant;
    }

const TypeMap<double>& HarmonicWallPotential::getShifts()
    {
    return shifts_;
    }

double HarmonicWallPotential::getShift(const std::string& type) const
    {
    return shifts_.at(type);
    }

void HarmonicWallPotential::setShifts(const TypeMap<double>& shifts)
    {
    shifts_ = TypeMap<double>(shifts);
    }

void HarmonicWallPotential::setShift(const std::string& type, double shift)
    {
    shifts_[type] = shift;
    }

}
