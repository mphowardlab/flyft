#include "_flyft.h"
#include "flyft/white_bear.h"
#include "flyft/white_bear_mark_ii.h"

void bindWhiteBearMarkII(py::module_& m)
    {
    using namespace flyft;

    py::class_<WhiteBearMarkII,std::shared_ptr<WhiteBearMarkII>,WhiteBear>(m, "WhiteBearMarkII")
        .def(py::init())
        ;
    }
