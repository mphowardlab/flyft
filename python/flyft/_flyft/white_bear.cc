#include "_flyft.h"
#include "flyft/rosenfeld_fmt.h"
#include "flyft/white_bear.h"

void bindWhiteBear(py::module_& m)
    {
    using namespace flyft;

    py::class_<WhiteBear,std::shared_ptr<WhiteBear>,RosenfeldFMT>(m, "WhiteBear")
        .def(py::init())
        ;
    }
