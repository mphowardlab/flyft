#ifndef FLYFT_COMPLEX_FIELD_H_
#define FLYFT_COMPLEX_FIELD_H_

#include "flyft/data_array.h"

#include <complex>

namespace flyft
    {

//! Complex-valued field.
/*!
 * A ComplexField represents a complex-valued quantity discretized onto a coordinate space. It is a
 * multidimensional DataArray and requires another object to define the coordinates.
 */
using ComplexField = DataArray<std::complex<double>, 4>;

    } // namespace flyft

#endif // FLYFT_COMPLEX_FIELD_H_
