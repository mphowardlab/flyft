#ifndef FLYFT_FIELD_H_
#define FLYFT_FIELD_H_

#include "flyft/data_array.h"

namespace flyft
    {

//! Real-valued field.
/*!
 * A Field represents a real-valued scalar quantity discretized onto a coordinate space. It is a
 * multidimensional DataArray and requires another object to define the coordinates.
 */
using Field = DataArray<double, 4>;

    } // namespace flyft

#endif // FLYFT_FIELD_H_
