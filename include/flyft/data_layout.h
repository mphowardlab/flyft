#ifndef FLYFT_DATA_LAYOUT_H_
#define FLYFT_DATA_LAYOUT_H_

#include <iterator>
#include <type_traits>
#include <vector>

namespace flyft
    {
//! Multidimensional array layout
/*!
 * DataLayout maps an N-dimensional index to one-dimensional
 * index using row-major ordering. Negative indexes are supported
 * and interpreted as relative to the zero-index.
 */
class DataLayout
    {
    public:
    DataLayout();
    //! Constructor
    /*!
     * \param shape Shape of the index array.
     */
    explicit DataLayout(const std::vector<int>& shape);

    //! Map a multidimensional index to a one-dimensional index
    /*!
     * \param idx Multidimensional index.
     * \return One-dimensional index.
     *
     * Row-major ordering is used.
     */
    int operator()(const std::vector<int>& idx) const;

    //! Shape of the layout in each dimension
    std::vector<int> shape() const;

    //! Total number of elements in the layout
    int size() const;

    //! Test if two layouts are equal
    /*!
     * \return True if the layouts have the same shape.
     */
    bool operator==(const DataLayout& other) const;

    //! Test if two layouts are not equal
    bool operator!=(const DataLayout& other) const;

    private:
    std::vector<int> shape_; //!< Multidimensional shape of layout
    };

    } // namespace flyft

#endif // FLYFT_DATA_LAYOUT_H_
