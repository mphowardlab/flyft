#include "flyft/solver.h"

namespace flyft
{

Solver::Solver(std::shared_ptr<GrandPotential> grand)
    : grand_(grand)
    {
    }

Solver::~Solver()
    {
    }
}
