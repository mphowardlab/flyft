#include "_flyft.h"
#include "flyft/external_potential.h"
#include "flyft/parameter.h"

template<class T>
auto bindWallPotential(py::module& m, const std::string& name)
    {
    py::class_<T, std::shared_ptr<T>, flyft::ExternalPotential> cls(m, name.c_str());
    cls.def(py::init<double, double>());
    cls.def(py::init<std::shared_ptr<flyft::DoubleParameter>, double>());
    cls.def_property(
        "origin",
        [](T& self) -> py::object
        {
            auto origin = self.getOrigin();
            auto const_origin = std::dynamic_pointer_cast<flyft::ConstantDoubleParameter>(origin);
            if (const_origin)
                {
                return py::cast(const_origin->getValue());
                }
            else
                {
                return py::cast(origin);
                }
        },
        [](T& self, py::object obj)
        {
            try
                {
                auto origin = obj.cast<double>();
                self.setOrigin(origin);
                }
            catch (const pybind11::cast_error& e)
                {
                auto origin = obj.cast<std::shared_ptr<flyft::DoubleParameter>>();
                self.setOrigin(origin);
                }
        });
    cls.def_property(
        "normal",
        [](T& self) -> py::object
        {
            auto normal = self.getNormal();
            auto const_normal = std::dynamic_pointer_cast<flyft::ConstantDoubleParameter>(normal);
            if (const_normal)
                {
                return py::cast(const_normal->getValue());
                }
            else
                {
                return py::cast(normal);
                }
        },
        [](T& self, py::object obj)
        {
            try
                {
                auto normal = obj.cast<double>();
                self.setNormal(normal);
                }
            catch (const pybind11::cast_error& e)
                {
                auto normal = obj.cast<std::shared_ptr<flyft::DoubleParameter>>();
                self.setNormal(normal);
                }
        });
    return cls;
    }
