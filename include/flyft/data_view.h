#ifndef FLYFT_DATA_VIEW_H_
#define FLYFT_DATA_VIEW_H_

#include "flyft/data_layout.h"

#include <type_traits>
#include <utility>

namespace flyft
    {

//! Accessor to a multidimensional array.
/*!
 * A DataView interprets a pointer as a multidimensional array according to a particular DataLayout.
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

        //! Construct from view.
        explicit Iterator(const DataView& view) : Iterator(view, nullptr) {}

        //! Construct from view starting from multidimensional index.
        Iterator(const DataView& view, int* current) : view_(view)
            {
            current_ = new int[view_.num_dimensions()];
            for (int dim = 0; dim < view_.num_dimensions(); ++dim)
                {
                current_[dim] = (current) ? current[dim] : 0;
                }
            }

        // non-copyable
        Iterator(const Iterator& other) = delete;
        Iterator& operator=(const Iterator& other) = delete;

        //! Move constructor
        Iterator(const Iterator&& other)
            : view_(std::move(other.view_)), current_(std::move(other.current_))
            {
            }

        //! Move assignment operator.
        Iterator& operator=(const Iterator&& other)
            {
            view_ = std::move(other.view_);
            current_ = std::move(other.current_);
            return *this;
            }

        //! Destructor.
        ~Iterator()
            {
            delete[] current_;
            }

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
            // increment the multi-index of the last dimension by 1
            int dim = view_.num_dimensions() - 1;
            ++current_[dim];

            // then, carry forward
            const auto view_shape = view_.shape();
            while (current_[dim] == view_shape[dim] && dim >= 1)
                {
                current_[dim] = 0;
                ++current_[dim--];
                }

            return *this;
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
        int* current_;
        };

    //! Empty constructor.
    DataView() : data_(nullptr), layout_(DataLayout()), start_(nullptr), shape_(nullptr) {}

    //! Constructor.
    /*!
     * \param data Pointer to the data.
     * \param layout Layout of the data.
     */
    DataView(pointer data, const DataLayout& layout) : data_(data), layout_(layout)
        {
        start_ = new int[layout_.num_dimensions()];
        shape_ = new int[layout_.num_dimensions()];
        const auto layout_shape = layout_.shape();
        for (int dim = 0; dim < layout_.num_dimensions(); ++dim)
            {
            start_[dim] = 0;
            shape_[dim] = layout_shape[dim];
            }
        }

    //! Scoped-view constructor.
    /*!
     * \param data_ Pointer to the data.
     * \param layout_ Layout of the data.
     * \param start_ Lower bound of multidimensional indexes within scope.
     * \param shape_ Shape of multidimensional indexes within scope, beginning with start.
     */
    DataView(pointer data, const DataLayout& layout, const int* start, const int* shape)
        : data_(data), layout_(layout)
        {
        start_ = new int[layout_.num_dimensions()];
        shape_ = new int[layout_.num_dimensions()];
        for (int dim = 0; dim < layout_.num_dimensions(); ++dim)
            {
            start_[dim] = start[dim];
            shape_[dim] = shape[dim];
            }
        }

    // non-copyable, non-movable
    DataView(const DataView<T>& other) = delete;
    DataView& operator=(const DataView<T>& other) = delete;
    DataView(const DataView<T>&& other) = delete;
    DataView& operator=(const DataView<T>&& other) = delete;

    //! Destructor.
    ~DataView()
        {
        delete[] start_;
        delete[] shape_;
        }

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
    int num_dimensions() const
        {
        return layout_.num_dimensions();
        }

    //! Number of elements per dimension in the scope of the view.
    const int* shape() const
        {
        return shape_;
        }

    //! Number of elements in the scope of the view.
    size_t size() const
        {
        size_t size;
        if (shape_)
            {
            size = 1;
            for (int dim = 0; dim < layout_.num_dimensions(); ++dim)
                {
                size *= shape_[dim];
                }
            }
        else
            {
            size = 0;
            }
        return size;
        }

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
     * \return Iterator to start of data.
     */
    Iterator begin() const
        {
        return Iterator(*this);
        }

    //! End point of the data
    /*!
     * \return Iterator to end of data.
     */
    Iterator end() const
        {
        return Iterator(*this, shape_);
        }

    private:
    pointer data_;      //!< Data.
    DataLayout layout_; //!< Multidimensional array layout.
    int* start_;        //!< Multi-index for start of scoped view.
    int* shape_;        //!< Number of elements per dimension of scoped view.
    };

    } // namespace flyft

#endif // FLYFT_DATA_VIEW_H_
