#ifndef FLYFT_SOLVER_H_
#define FLYFT_SOLVER_H_

#include "flyft/free_energy_functional.h"
#include "flyft/state.h"

#include <memory>

namespace flyft
{

class Solver
    {
    public:
        Solver();
        Solver(std::shared_ptr<FreeEnergyFunctional> excess);
        Solver(std::shared_ptr<FreeEnergyFunctional> excess,
               std::shared_ptr<FreeEnergyFunctional> external);
        virtual ~Solver();

        // noncopyable / nonmovable
        Solver(const Solver&) = delete;
        Solver(Solver&&) = delete;
        Solver& operator=(const Solver&) = delete;
        Solver& operator=(Solver&&) = delete;

        virtual bool solve(std::shared_ptr<State> state) = 0;

        std::shared_ptr<FreeEnergyFunctional> getExcessFunctional();
        std::shared_ptr<const FreeEnergyFunctional> getExcessFunctional() const;
        void setExcessFunctional(std::shared_ptr<FreeEnergyFunctional> excess);

        std::shared_ptr<FreeEnergyFunctional> getExternalFunctional();
        std::shared_ptr<const FreeEnergyFunctional> getExternalFunctional() const;
        void setExternalFunctional(std::shared_ptr<FreeEnergyFunctional> excess);

    protected:
        std::shared_ptr<FreeEnergyFunctional> excess_;
        std::shared_ptr<FreeEnergyFunctional> external_;
    };

}

#endif // FLYFT_SOLVER_H_
