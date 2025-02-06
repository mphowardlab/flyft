#ifndef FLYFT_BOUNDARY_TYPE_H_
#define FLYFT_BOUNDARY_TYPE_H_

namespace flyft
    {
enum class BoundaryType
{
    periodic,
    zero,
    reflect,
    repeat,
    internal
};
    } // namespace flyft

#endif // FLYFT_BOUNDARY_TYPE_H_
