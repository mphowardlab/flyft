#ifndef FLYFT_DATA_VIEW_H_
#define FLYFT_DATA_VIEW_H_

#include "flyft/data_layout.h"

#include <algorithm>
#include <functional>

namespace flyft
    {

template<typename T>
//! Multidimensional array view
/*!
 * DataView provides ability to view a certain section of the multidimensional array
 */
class DataView
    {
    public:
    using value_type = typename std::remove_reference<T>::type; // Extracts datatype
    using pointer = value_type*;                                // Pointer to the datatype
    using reference = value_type&;                              // Reference to the datatype

    class Iterator
        {
        public:
        using iterator_category = std::bidirectional_iterator_tag;
        using difference_type = int;
        using value_type = DataView::value_type;
        using pointer = DataView::pointer;
        using reference = DataView::reference;

        // Overloading the constructor of the iterator class
        Iterator() : Iterator(DataView()) {}

        explicit Iterator(const DataView& view) : Iterator(view, 0) {}

        Iterator(const DataView& view, int current) : view_(view), current_(current) {}

        reference operator*() const
            {
            return view_(current_);
            }

        pointer operator->() const
            {
            return get();
            }

        pointer get() const
            {
            return &view_(current_);
            }

        Iterator& operator++()
            {
            ++current_;
            return *this;
            }

        Iterator operator++(int)
            {
            Iterator tmp(*this);
            ++current_;
            return tmp;
            }

        Iterator& operator--()
            {
            --current_;
            return *this;
            }

        Iterator operator--(int)
            {
            Iterator tmp(*this);
            --current_;
            return tmp;
            }

        bool operator==(const Iterator& other) const
            {
            return (get() == other.get());
            }

        bool operator!=(const Iterator& other) const
            {
            return !(*this == other);
            }

        private:
        DataView view_;
        int current_;
        };

    //! Empty constructor
    /*!
     * The default is a null pointer to a zero-size layout.
     */
    DataView() : DataView(nullptr, DataLayout()) {}

    //! Constructor
    /*!
     * \param data_ Pointer to the data.
     * \param layout_ Layout of the data.
     */

    DataView(pointer data, const DataLayout& layout) : DataView(data, layout, 0, layout.shape()) {}

    //! Constructor
    /*!
     * \param data_ Pointer to the data.
     * \param layout_ Layout of the data.
     * \param start_ Start of the array.
     * \param end_ End of the array.
     *
     * Initializes the view of the multi-dimensional array.
     */
    DataView(pointer data, const DataLayout& layout, std::vector<int> start, std::vector<int> end)
        : data_(data), layout_(layout), start_(start), end_(end)
        {
        }

    //! Map multi-dimensional array to one dimension
    /*!
     * \param idx Array of indices.
     * \return One dimensional map of the multidimensional array.
     */
    reference operator()(const std::vector<int>& idx) const
        {
        std::vector<int> temp;
        std::transform(idx.begin(), idx.end(), start_, temp.begin(), std::plus<int>());
        return data_[layout_(temp)];
        }

    //! Shape of the multi-dimensional array
    std::vector<int> shape() const
        {
        if (start_.size() != end_.size())
            {
            throw std::invalid_argument(
                "Arrays must have the same size for element-wise subtraction.");
            }

        std::vector<int>& result(start_.size());
        for (int i = 0; i < start_.size(); ++i)
            {
            result[i] = start_[i] - end_[i];
            }
        return result;
        }

    //! Number of elements in the multi-dimensional array
    int size() const
        {
        return std::accumulate(shape().begin(), shape().end(), 1, std::multiplies<int>());
        }

    //! Test if pointer to the data is not a null pointer
    /*!
     * \return DataView is not null if it is a view of valid data.
     */
    explicit operator bool() const
        {
        return (data_ != nullptr);
        }

    //! Start point of the data
    /*!
     * \return data at index position 0.
     */
    Iterator begin() const
        {
        return Iterator(*this, 0);
        }

    //! End point of the data
    /*!
     * \return data at end of the array.
     */
    Iterator end() const
        {
        return Iterator(*this, shape());
        }

    //! Test if two views are equal
    /*!
     * \return True if the view have the same pointer, layout, start and end points.
     */
    bool operator==(const DataView& other) const
        {
        return (data_ == other.data_ && layout_ == other.layout_ && start_ == other.start_
                && end_ == other.end_);
        }

    //! Test if two views are not equal
    bool operator!=(const DataView& other) const
        {
        return !(*this == other);
        }

    private:
    pointer data_;           //!< Pointer to the array
    DataLayout layout_;      //!< Multidimensional array layout
    std::vector<int> start_; //!< Start of the array
    std::vector<int> end_;   //!< End of the array
    };

    } // namespace flyft

#endif // FLYFT_DATA_VIEW_H_
