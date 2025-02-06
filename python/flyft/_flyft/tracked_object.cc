#include "flyft/tracked_object.h"

#include "_flyft.h"

void bindTrackedObject(py::module_& m)
    {
    using namespace flyft;

    py::class_<TrackedObject, std::shared_ptr<TrackedObject>> obj(m, "TrackedObject");
    obj.def(py::init<>());
    obj.def_property_readonly("id", &TrackedObject::id);

    py::class_<TrackedObject::Token, std::shared_ptr<TrackedObject::Token>>(obj, "Token")
        .def_property_readonly("dirty", &TrackedObject::Token::dirty)
        .def("__eq__",
             [](const TrackedObject::Token& self, const TrackedObject::Token& other)
             { return self == other; })
        .def("__neq__",
             [](const TrackedObject::Token& self, const TrackedObject::Token& other)
             { return self != other; });
    }
