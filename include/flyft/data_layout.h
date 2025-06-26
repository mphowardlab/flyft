#ifndef FLYFT_DATA_LAYOUT_H_
#define FLYFT_DATA_LAYOUT_H_

#include <stddef.h>

namespace flyft
    {

//! Memory layout of a multidimensional array.
/*!
 * A multidimensional array has a number of dimensions \a N. Its shape is the number of elements
 * along each dimension, and its size is the total number of elements (product of the shape). An
 * element of the array can be accessed by a multi-index, which is a length \a N array. The array is
 * layed out contiguously in one-dimensional memory using generalized row-major ordering, meaning
 * that the first index is the "slowest" varying and the last index is the "fastest" varying.
 */
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
    DataLayout(char num_dimensions, const int* shape);

    //! Copy constructor
    DataLayout(const DataLayout& other);

    //! Copy assignment operator
    DataLayout& operator=(const DataLayout& other);

    //! Move constructor
    DataLayout(const DataLayout&& other);

    //! Move assignment operator
    DataLayout& operator=(const DataLayout&& other);

    //! Destructor
    ~DataLayout();

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

    //! Map a one-dimensional index to a multidimensional index offset.
    /*!
     * \param multi_index Multidimensional index.
     * \param flat_index One-dimensional index.
     * \param offset Offset for multidimensional index.
     */
    void operator()(int* multi_index, size_t flat_index, const int* offset) const;

    //! Number of dimensions.
    char num_dimensions() const;

    //! Number of elements per dimension.
    const int* shape() const;

    //! Total number of elements.
    size_t size() const;

    //! Test if two layouts are equal.
    bool operator==(const DataLayout& other) const;

    //! Test if two layouts are not equal.
    bool operator!=(const DataLayout& other) const;

    private:
    char num_dimensions_; //!< Number of dimensions.
    int* shape_;          //!< Number of elements per dimension.
    size_t size_;         //!< Total number of elements.

    //! Reset shape and dimensionality of layout
    void reset(char num_dimensions, const int* shape);
    };

    } // namespace flyft

#endif // FLYFT_DATA_LAYOUT_H_
