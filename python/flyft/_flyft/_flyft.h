#ifndef FLYFT_PYTHON__FLYFT_H_
#define FLYFT_PYTHON__FLYFT_H_

// flyft headers for binding opaque types
#include "flyft/field.h"
#include "flyft/grand_potential.h"
#include "flyft/type_map.h"

#include <pybind11/pybind11.h>
namespace py = pybind11;

#include <memory>
#include <string>
#include <vector>

// make opaque types for accessing properties elsewhere
PYBIND11_MAKE_OPAQUE(std::vector<std::string>);
PYBIND11_MAKE_OPAQUE(flyft::TypeMap<double>);
PYBIND11_MAKE_OPAQUE(flyft::TypeMap<flyft::Field>);
PYBIND11_MAKE_OPAQUE(flyft::TypeMap<flyft::GrandPotential::Constraint>);

#endif // FLYFT_PYTHON__FLYFT_H_
