#include "flyft/communicator.h"

#include "_flyft.h"

void bindCommunicator(py::module_& m)
    {
    using namespace flyft;

    py::class_<Communicator, std::shared_ptr<Communicator>>(m, "Communicator")
        // TODO: support constructing with user's MPI_Comm
        .def(py::init<>())
        .def_property_readonly("size", &Communicator::size)
        .def_property_readonly("rank", &Communicator::rank)
        .def_property_readonly("root", &Communicator::root);
    }
