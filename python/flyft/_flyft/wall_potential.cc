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
        .def(py::init<std::shared_ptr<DoubleParameter>,double>())
        .def_property("origin",
                      [](WallPotential& self) -> py::object {
                        auto origin = self.getOrigin();
                        auto const_origin = std::dynamic_pointer_cast<ConstantDoubleParameter>(origin);
                        if (const_origin)
                            {
                            return py::cast(const_origin->getValue());
                            }
                        else
                            {
                            return py::cast(origin);
                            }
                      },
                      [](WallPotential& self, py::object obj) {
                        try
                            {
                            auto origin = obj.cast<double>();
                            self.setOrigin(origin);
                            }
                        catch (const pybind11::cast_error& e)
                            {
                            auto origin = obj.cast<std::shared_ptr<DoubleParameter>>();
                            self.setOrigin(origin);
                            }
                      }
                      )
        .def_property("normal",
                      [](WallPotential& self) -> py::object {
                        auto normal = self.getNormal();
                        auto const_normal = std::dynamic_pointer_cast<ConstantDoubleParameter>(normal);
                        if (const_normal)
                            {
                            return py::cast(const_normal->getValue());
                            }
                        else
                            {
                            return py::cast(normal);
                            }
                      },
                      [](WallPotential& self, py::object obj) {
                        try
                            {
                            auto normal = obj.cast<double>();
                            self.setNormal(normal);
                            }
                        catch (const pybind11::cast_error& e)
                            {
                            auto normal = obj.cast<std::shared_ptr<DoubleParameter>>();
                            self.setNormal(normal);
                            }
                      }
                      )
        ;
    }
