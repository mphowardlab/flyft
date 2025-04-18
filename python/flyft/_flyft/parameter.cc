#include "flyft/parameter.h"

#include "_flyft.h"

namespace flyft
    {

template<typename T>
class ParameterTrampoline : public Parameter<T>
    {
    public:
    using Parameter<T>::Parameter;

    T operator()(std::shared_ptr<State> state) override
        {
        PYBIND11_OVERRIDE_PURE_NAME(T, Parameter<T>, "__call__", operator(), state);
        }
    };
    } // namespace flyft

void bindDoubleParameters(py::module_& m)
    {
    using namespace flyft;

    py::class_<DoubleParameter, std::shared_ptr<DoubleParameter>, ParameterTrampoline<double>>(
        m,
        "DoubleParameter")
        .def(py::init<>())
        .def("__call__", &DoubleParameter::operator());

    py::class_<LinearParameter, std::shared_ptr<LinearParameter>, DoubleParameter>(
        m,
        "LinearParameter")
        .def(py::init<double, double, double>())
        .def_property("initial", &LinearParameter::getInitial, &LinearParameter::setInitial)
        .def_property("origin", &LinearParameter::getOrigin, &LinearParameter::setOrigin)
        .def_property("rate", &LinearParameter::getRate, &LinearParameter::setRate);

    py::class_<SquareRootParameter, std::shared_ptr<SquareRootParameter>, DoubleParameter>(
        m,
        "SquareRootParameter")
        .def(py::init<double, double, double>())
        .def_property("initial", &SquareRootParameter::getInitial, &SquareRootParameter::setInitial)
        .def_property("origin", &SquareRootParameter::getOrigin, &SquareRootParameter::setOrigin)
        .def_property("rate", &SquareRootParameter::getRate, &SquareRootParameter::setRate);
    }
