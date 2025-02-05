#include "flyft/solver.h"

#include "_flyft.h"

namespace flyft
    {
class SolverTrampoline : public Solver
    {
    public:
    using Solver::Solver;

    bool solve(std::shared_ptr<GrandPotential> grand, std::shared_ptr<State> state) override
        {
        PYBIND11_OVERRIDE_PURE(bool, Solver, solve, grand, state);
        }
    };
    } // namespace flyft

void bindSolver(py::module_& m)
    {
    using namespace flyft;

    py::class_<Solver, std::shared_ptr<Solver>, SolverTrampoline>(m, "Solver")
        .def(py::init<>())
        .def("solve", &Solver::solve);
    }
