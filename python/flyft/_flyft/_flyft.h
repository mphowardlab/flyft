#ifndef FLYFT_PYTHON__FLYFT_H_
#define FLYFT_PYTHON__FLYFT_H_

// flyft headers for binding opaque types
#include "flyft/field.h"
#include "flyft/flux.h"
#include "flyft/functional.h"
#include "flyft/grand_potential.h"
#include "flyft/pair_map.h"
#include "flyft/type_map.h"

#include <pybind11/pybind11.h>
namespace py = pybind11;

#include <memory>
#include <string>
#include <vector>

// make opaque types for accessing properties elsewhere
PYBIND11_MAKE_OPAQUE(std::vector<std::string>);
PYBIND11_MAKE_OPAQUE(std::vector<flyft::Flux>);
PYBIND11_MAKE_OPAQUE(std::vector<flyft::Functional>);
PYBIND11_MAKE_OPAQUE(std::vector<flyft::ExternalPotential>);
PYBIND11_MAKE_OPAQUE(flyft::TypeMap<bool>);
PYBIND11_MAKE_OPAQUE(flyft::TypeMap<double>);
PYBIND11_MAKE_OPAQUE(flyft::TypeMap<flyft::Field>);
PYBIND11_MAKE_OPAQUE(flyft::TypeMap<flyft::GrandPotential::Constraint>);
PYBIND11_MAKE_OPAQUE(flyft::PairMap<double>);

#endif // FLYFT_PYTHON__FLYFT_H_
