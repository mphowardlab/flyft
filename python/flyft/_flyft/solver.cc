#include "_flyft.h"
#include "flyft/solver.h"

namespace flyft
{
class SolverTrampoline : public Solver
    {
    public:
        using Solver::Solver;

        bool solve(std::shared_ptr<State> state) override
            {
            PYBIND11_OVERRIDE_PURE(bool, Solver, solve, state);
            }
    };
}

void bindSolver(py::module_& m)
    {
    using namespace flyft;

    py::class_<Solver,std::shared_ptr<Solver>,SolverTrampoline>(m, "Solver")
        .def(py::init<>())
        .def("solve", &Solver::solve)
        .def_property("excess", py::overload_cast<>(&Solver::getExcessFunctional), &Solver::setExcessFunctional)
        .def_property("external", py::overload_cast<>(&Solver::getExternalFunctional), &Solver::setExternalFunctional)
        ;
    }
