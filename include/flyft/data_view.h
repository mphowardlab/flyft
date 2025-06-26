#ifndef FLYFT_DATA_VIEW_H_
#define FLYFT_DATA_VIEW_H_

#include "flyft/data_layout.h"

namespace flyft
    {

//! Accessor to a multidimensional array.
/*!
 * A Dataview interprets a pointer as a multidimensional array according to a particular DataLayout.
 * It provides random access to elements by multi-index, as well as a forward iterator.
 *
 * The view can be scoped to a subset of only some of the valid multi-indexes. This is useful for
 * accessing only the elements of the array that are of interest when the array is padded. When the
 * view is scoped, negative indexes can be used and are interpreted relative to the starting
 * multi-index of the scope. One use of negative indexes, in conjunction with padding, is to apply
 * finite-difference stencils near edges without special-case handling. It is assumed that the
 * resulting index is valid for the layout.
 */
template<typename T>
class DataView
    {
    public:
    using value_type = typename std::remove_reference<T>::type; //<! Data type
    using pointer = value_type*;                                //<! Pointer to data type
    using reference = value_type&;                              //<! Reference to data type

    class Iterator
        {
        public:
        using value_type = DataView::value_type; //!< Data type
        using pointer = DataView::pointer;       //!< Pointer to data type
        using reference = DataView::reference;   //!< Reference to data type

        //! Empty constructor
        Iterator() : Iterator(DataView()) {}

        //! Construct from view
        explicit Iterator(const DataView& view) : Iterator(view, 0) {}

        Iterator(const DataView& view, size_t current) : view_(view), current_(current) {}

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
        size_t current_flat_index;
        };

    //! Empty constructor.
    DataView() : data_(nullptr), layout_(DataLayout()), start_(nullptr), end_(nullptr) {}

    //! Constructor.
    /*!
     * \param data Pointer to the data.
     * \param layout Layout of the data.
     */
    DataView(pointer data, const DataLayout& layout)
        : data_(data), layout_(layout), start_(nullptr), end_(nullptr)
        {
        }

    //! Scoped-view constructor.
    /*!
     * \param data_ Pointer to the data.
     * \param layout_ Layout of the data.
     * \param start_ Lower bound of multidimensional indexes within scope.
     * \param end_ Upper bound of multidimensional indexes within scope.
     */
    DataView(pointer data, const DataLayout& layout, const int* start, const int* end)
        : data_(data), layout_(layout), start_(start), end_(end)
        {
        }

    // non-copyable, non-movable
    DataView(const DataView<T>& other) = delete;
    DataView& operator=(const DataView<T>& other) = delete;
    DataView(const DataView<T>&& other) = delete;
    DataView& operator=(const DataView<T>&& other) = delete;

    //! Destructor.
    ~DataView() {}

    //! Access an element of the data at a given multidimensional index.
    /*!
     * \param multi_index Multidimensional index to access.
     * \return Data element.
     */
    reference operator()(const int* multi_index) const
        {
        return data_[layout_(multi_index, start_)];
        }

    //! Number of dimensions.
    char num_dimensions() const
        {
        return layout_.num_dimensions();
        }

#if 0
    //! Shape of the multi-dimensional array
    std::vector<int> shape() const
        {
        std::vector<int>& result(layout_.num_dimensions());
        for (char dim = 0; dim < layout_.num_dimensions(); ++dim)
            {
            result[i] = end_[dim] - start_[dim];
            }
        return result;
        }

    //! Number of elements in the multi-dimensional array
    int size() const
        {
        return std::accumulate(shape().begin(), shape().end(), 1, std::multiplies<int>());
        }
#endif

    //! Test if view is not null
    /*!
     * \returns True if data pointer being viewed is not null.
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

    private:
    pointer data_;      //!< Pointer to the array
    DataLayout layout_; //!< Multidimensional array layout
    int* start_;        //!< Start of scoped view
    int* end_;          //!< End of scoped view
    };

    } // namespace flyft

#endif // FLYFT_DATA_VIEW_H_
