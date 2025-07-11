#ifndef FLYFT_DATA_LAYOUT_H_
#define FLYFT_DATA_LAYOUT_H_

#include <assert.h>
#include <stddef.h>

namespace flyft
    {

//! Memory layout of a multidimensional array.
/*!
 * A multidimensional array has a number of dimensions \a n. Its shape is the number of elements
 * along each dimension, and its size is the total number of elements (product of the shape). An
 * element of the array can be accessed by a multidimensional index, which is a length \a n array.
 * The array is layed out contiguously in one-dimensional memory using generalized row-major
 * ordering, meaning that the first index is the "slowest" varying and the last index is the
 * "fastest" varying.
 *
 * The maximum dimension that can be represented by the layout is set by the template parameter
 * \a N.
 */
template<int N>
class DataLayout
    {
    public:
    //! Empty constructor
    DataLayout();

    //! Constructor
    /*!
     * \param num_dimensions Number of dimensions.
     * \param shape Number of elements per dimension.
     */
    DataLayout(int num_dimensions, const int* shape);

    //! Map a multidimensional index to a one-dimensional index.
    /*!
     * \param multi_index Multidimensional index.
     * \return One-dimensional index.
     */
    size_t operator()(const int* multi_index) const;

    //! Map a multidimensional index offset by another into a one-dimensional index.
    /*!
     * \param multi_index Multidimensional index.
     * \param offset Offset for multidimensional index.
     * \return One-dimensional index.
     *
     * The total multidimensional index is assumed to be valid.
     */
    size_t operator()(const int* multi_index, const int* offset) const;

    //! Map a one-dimensional index to a multidimensional index.
    /*!
     * \param multi_index Multidimensional index.
     * \param flat_index One-dimensional index.
     */
    void operator()(int* multi_index, size_t flat_index) const;

    //! Map a one-dimensional index to a multidimensional index relative to an offset.
    /*!
     * \param multi_index Multidimensional index.
     * \param flat_index One-dimensional index.
     * \param offset Offset for multidimensional index.
     */
    void operator()(int* multi_index, size_t flat_index, const int* offset) const;

    //! Number of dimensions.
    int num_dimensions() const;

    //! Number of elements per dimension.
    const int* shape() const;

    //! Total number of elements.
    size_t size() const;

    //! Test if two layouts are equal.
    template<int M>
    bool operator==(const DataLayout<M>& other) const;

    //! Test if two layouts are not equal.
    template<int M>
    bool operator!=(const DataLayout<M>& other) const;

    static int constexpr MAX_NUM_DIMENSIONS = N; //!< Maximum number of dimensions.

    private:
    int num_dimensions_; //!< Number of dimensions.
    int shape_[N];       //!< Number of elements per dimension.
    size_t stride_[N];   //!< Number of elements to advance when incrementing index.
    size_t size_;        //!< Total number of elements.
    };

template<int N>
DataLayout<N>::DataLayout() : num_dimensions_(0), shape_ {0}, stride_ {0}, size_(0)
    {
    }

template<int N>
DataLayout<N>::DataLayout(int num_dimensions, const int* shape) : num_dimensions_(num_dimensions)
    {
    assert(num_dimensions > 0 && num_dimensions_ <= N);
    assert(shape);

    // row-major ordering
    size_t stride = 1;
    for (int dim = num_dimensions - 1; dim >= 0; --dim)
        {
        assert(shape[dim] >= 1);

        shape_[dim] = shape[dim];
        stride_[dim] = stride;
        stride *= shape_[dim];
        }

    // final value of stride will be the full size
    size_ = stride;
    }

template<int N>
size_t DataLayout<N>::operator()(const int* multi_index) const
    {
    assert(multi_index);

    size_t flat_index;
    if (num_dimensions_ > 1)
        {
        flat_index = 0;
        for (int dim = 0; dim < num_dimensions_; ++dim)
            {
            flat_index += multi_index[dim] * stride_[dim];
            }
        }
    else
        {
        flat_index = multi_index[0];
        }
    return flat_index;
    }

template<int N>
size_t DataLayout<N>::operator()(const int* multi_index, const int* offset) const
    {
    // use method without addition if there is no starting index.
    if (!offset)
        {
        return this->operator()(multi_index);
        }

    // otherwise, do calculation with starting offset added
    assert(multi_index);
    assert(offset);
    size_t flat_index;
    if (num_dimensions_ > 1)
        {
        flat_index = 0;
        for (int dim = 0; dim < num_dimensions_; ++dim)
            {
            flat_index += (offset[dim] + multi_index[dim]) * stride_[dim];
            }
        }
    else
        {
        flat_index = offset[0] + multi_index[0];
        }
    return flat_index;
    }

template<int N>
void DataLayout<N>::operator()(int* multi_index, size_t flat_index) const
    {
    assert(multi_index);

    if (num_dimensions_ > 1)
        {
        // repeatedly floor divide the flat index by stride associated with it
        size_t idx = flat_index;
        for (int dim = 0; dim < num_dimensions_; ++dim)
            {
            multi_index[dim] = idx / stride_[dim];
            idx -= multi_index[dim] * stride_[dim];
            }
        }
    else
        {
        multi_index[0] = flat_index;
        }
    }

template<int N>
void DataLayout<N>::operator()(int* multi_index, size_t flat_index, const int* offset) const
    {
    this->operator()(multi_index, flat_index);
    if (offset)
        {
        for (int dim = 0; dim < num_dimensions_; ++dim)
            {
            multi_index[dim] -= offset[dim];
            }
        }
    }

template<int N>
int DataLayout<N>::num_dimensions() const
    {
    return num_dimensions_;
    }

template<int N>
const int* DataLayout<N>::shape() const
    {
    return shape_;
    }

template<int N>
size_t DataLayout<N>::size() const
    {
    return size_;
    }

template<int N>
template<int M>
bool DataLayout<N>::operator==(const DataLayout<M>& other) const
    {
    // dimensionality & size must match as first pass
    bool same_layout = (num_dimensions_ == other.num_dimensions_ && size_ == other.size_);
    if (same_layout)
        {
        for (int dim = 0; dim < num_dimensions_ && same_layout; ++dim)
            {
            if (shape_[dim] != other.shape_[dim])
                {
                same_layout = false;
                }
            }
        }

    return same_layout;
    }

template<int N>
template<int M>
bool DataLayout<N>::operator!=(const DataLayout<M>& other) const
    {
    return !(*this == other);
    }

    } // namespace flyft

#endif // FLYFT_DATA_LAYOUT_H_
