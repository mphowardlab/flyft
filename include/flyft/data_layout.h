#ifndef FLYFT_DATA_LAYOUT_H_
#define FLYFT_DATA_LAYOUT_H_

#include <iterator>
#include <type_traits>
#include <vector>

namespace flyft
    {
//! DataLayout
/*!
Generates data structure for the custom data types used in the software.
The class provides get and set functions for the array index and implements N-dimensional array
mapping into one dimensional array using strided indexing scheme.
*/
class DataLayout
    {
    public:
    DataLayout();
    //! Constructor for the DataLayout
    /*!
     * \param shape_ Shape of the index array
     */
    explicit DataLayout(const std::vector<int>& shape);

    // Getter for the class
    int operator()(const std::vector<int>& idx) const;

    // Setter for shape
    std::vector<int> shape() const;
    // Getter for shape
    int size() const;

    bool operator==(const DataLayout& other) const;
    bool operator!=(const DataLayout& other) const;

    private:
    std::vector<int> shape_;
    };

    } // namespace flyft

#endif // FLYFT_DATA_LAYOUT_H_
