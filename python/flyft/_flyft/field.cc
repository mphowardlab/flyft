#include "_flyft.h"
#include "flyft/field.h"

void bindField(py::module_& m)
    {
    using namespace flyft;

    py::class_<Field,std::shared_ptr<Field>>(m, "Field", py::buffer_protocol())
        .def(py::init<int>())
        .def_property_readonly("shape", &Field::shape)
        .def_property_readonly("buffer_shape", &Field::buffer_shape)
        .def_property_readonly("buffered_shape", &Field::buffered_shape)
        .def("__getitem__", [](const Field &f, int idx) {
            if (idx < 0 || idx >= f.buffered_shape())
                throw py::index_error();
            return f(idx);
        })
       .def("__setitem__", [](Field &f, int idx, double value) {
            if (idx < 0 || idx >= f.buffered_shape())
                throw py::index_error();
            f(idx) = value;
        })
        .def_buffer([](Field &f) -> py::buffer_info {
            return py::buffer_info(
                f.begin(),
                sizeof(double),
                py::format_descriptor<double>::format(),
                1,
                {f.buffered_shape()},
                {sizeof(double)}
                );
        })
        ;
    }
