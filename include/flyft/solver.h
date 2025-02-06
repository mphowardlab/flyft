#ifndef FLYFT_SOLVER_H_
#define FLYFT_SOLVER_H_

#include "flyft/grand_potential.h"
#include "flyft/state.h"

#include <memory>

namespace flyft
    {

class Solver
    {
    public:
    Solver();
    virtual ~Solver();

    // noncopyable / nonmovable
    Solver(const Solver&) = delete;
    Solver(Solver&&) = delete;
    Solver& operator=(const Solver&) = delete;
    Solver& operator=(Solver&&) = delete;

    virtual bool solve(std::shared_ptr<GrandPotential> grand, std::shared_ptr<State> state) = 0;
    };

    } // namespace flyft

#endif // FLYFT_SOLVER_H_
