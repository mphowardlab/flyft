#include "_flyft.h"
#include "flyft/mesh.h"
#include "flyft/boundary_type.h"
namespace flyft
{

class MeshTrampoline : public Mesh
    {
    public:
        using Mesh::Mesh;

        std::shared_ptr<Mesh> slice(int start, int end) const override
            {
            PYBIND11_OVERRIDE_PURE(std::shared_ptr<Mesh>, Mesh, slice, start, end);
            }
        
        double area(int i) const override
            {
            PYBIND11_OVERRIDE_PURE(double, Mesh, area, i);
            }
        
        double volume() const override 
            {
            PYBIND11_OVERRIDE_PURE(double, Mesh, volume);
            }
     
        double volume(int i) const override
            {
            PYBIND11_OVERRIDE_PURE_NAME(double, Mesh, "bin_volume",volume, i);
            }

        double gradient(int idx, double f_lo, double f_hi) const override
            {
            PYBIND11_OVERRIDE_PURE(double, Mesh, gradient, idx, f_lo, f_hi);
            }
    };

}

void bindMesh(py::module_& m)
    {
    using namespace flyft;

    py::class_<Mesh,std::shared_ptr<Mesh>,MeshTrampoline>(m, "Mesh")
        .def(py::init<double,int,BoundaryType, BoundaryType>())
        .def(py::init<double,int,double,BoundaryType,BoundaryType>())
        .def(py::init<int,double>())
        .def(py::init<int,double,double>())
        .def_property_readonly("L", &Mesh::L)
        .def_property_readonly("shape", &Mesh::shape)
        .def_property_readonly("step", &Mesh::step)
        .def_property_readonly("origin", &Mesh::origin)
        .def("center", &Mesh::center)
        .def("bin", &Mesh::bin)
        .def("lower_bound",&Mesh::lower_bound)
        .def("upper_bound",&Mesh::upper_bound)
        .def_property_readonly("volume",py::overload_cast<>(&Mesh::volume, py::const_))
        .def("bin_volume",py::overload_cast<int>(&Mesh::volume, py::const_))
        ;
    }
