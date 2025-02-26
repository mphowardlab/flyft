#ifndef FLYFT_DATA_VIEW_H_
#define FLYFT_DATA_VIEW_H_

#include "flyft/data_layout.h"

#include <algorithm>
#include <functional>

namespace flyft
    {

template<typename T>
//! DataView
/*!
 * Creates ability to view a certain section of the N-dimensional array
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

    //! Overloading the constructor of the DataView class
    DataView() : DataView(nullptr, DataLayout()) {}

    DataView(pointer data, const DataLayout& layout) : DataView(data, layout, 0, layout.shape()) {}

    DataView(pointer data, const DataLayout& layout, int start, int end)
        : data_(data), layout_(layout), start_(start), end_(end)
        {
        }

    //! Mapping the N-dimensional index to 1D index
    /*!
     * \param idx Array of indices
     */
    reference operator()(const std::vector<int>& idx) const
        {
        std::vector<int>& temp;
        std::transform(idx.begin(), idx.end(), start_, temp.begin(), std::plus<int>());
        return data_[layout_(temp)];
        }

    int shape() const
        {
        return end_ - start_;
        }

    int size() const
        {
        return shape();
        }

    explicit operator bool() const
        {
        return (data_ != nullptr);
        }

    Iterator begin() const
        {
        return Iterator(*this, 0);
        }

    Iterator end() const
        {
        return Iterator(*this, shape());
        }

    bool operator==(const DataView& other) const
        {
        return (data_ == other.data_ && layout_ == other.layout_ && start_ == other.start_
                && end_ == other.end_);
        }

    bool operator!=(const DataView& other) const
        {
        return !(*this == other);
        }

    private:
    pointer data_;
    DataLayout layout_;
    int start_;
    int end_;
    };

    } // namespace flyft

#endif // FLYFT_DATA_VIEW_H_
