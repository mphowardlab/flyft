#ifndef FLYFT_DATA_LAYOUT_H_
#define FLYFT_DATA_LAYOUT_H_

#include <iterator>
#include <type_traits>
#include <vector>

namespace flyft
    {
class DataLayout
    {
    //! DataLayout class generates the template for the custom data types used in the software
    //! The class provides get and set functions for the array index and
    //! implements bool operations for the data types
    public:
    DataLayout();
    //! Constructor for the DataLayout
    explicit DataLayout(const std::vector<int>& shape);

    // Getter for the class
    int operator()(const std::vector<int>& idx) const;

    // Setter for shape
    std::vector<int> shape() const;
    // Getter for shape
    std::vector<int> size() const;

    bool operator==(const DataLayout& other) const;
    bool operator!=(const DataLayout& other) const;

    private:
    std::vector<int> shape_;
    };

    } // namespace flyft

#endif // FLYFT_DATA_LAYOUT_H_
