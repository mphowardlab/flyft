#include "_flyft.h"
#include "flyft/wall_potential.h"

namespace flyft
{
//! Trampoline for WallPotential to python
class WallPotentialTrampoline : public WallPotential
    {
    public:
        // Pull in constructors
        using WallPotential::WallPotential;

        //! pybind11 override of pure virtual compute method
        void potential(std::shared_ptr<Field> V,
                       const std::string& type,
                       std::shared_ptr<State> state) override
            {
            PYBIND11_OVERRIDE_PURE(void, WallPotential, potential, V, type, state);
            }
    };
}

void bindWallPotential(py::module_& m)
    {
    using namespace flyft;

    py::class_<WallPotential,std::shared_ptr<WallPotential>,WallPotentialTrampoline,ExternalPotential>(m, "WallPotential")
        .def(py::init<double,double>())
        .def_property("origin", &WallPotential::getOrigin, &WallPotential::setOrigin)
        .def_property("normal", &WallPotential::getNormal, &WallPotential::setNormal)
        ;
    }
