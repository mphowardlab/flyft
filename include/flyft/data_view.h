#ifndef FLYFT_DATA_VIEW_H_
#define FLYFT_DATA_VIEW_H_

#include "flyft/data_layout.h"

#include <assert.h>
#include <iterator>
#include <type_traits>
#include <utility>

namespace flyft
    {

//! Accessor to a multidimensional array.
/*!
 * A DataView interprets a pointer as a multidimensional array according to a particular DataLayout.
 * It provides random access to elements by multidimensional index, as well as a forward iterator.
 *
 * The view can be scoped to a subset of only some of the valid multidimensional indexes. This is
 * useful for accessing only the elements of the array that are of interest when the array is
 * padded. When the view is scoped, negative indexes can be used and are interpreted relative to the
 * starting multidimensional index for the scope. One use of negative indexes, in conjunction with
 * padding, is to apply finite-difference stencils near edges without special-case handling. It is
 * assumed that the resulting index is valid for the layout.
 *
 * The maximum dimension that can be represented by the layout is set by the template parameter
 * \a N.
 */
template<typename T, int N>
class DataView
    {
    public:
    using value_type = typename std::remove_reference<T>::type; //<! Data type
    using pointer = value_type*;                                //<! Pointer to data type
    using reference = value_type&;                              //<! Reference to data type

    using Layout = DataLayout<N>; //!< Layout type

    class Iterator
        {
        public:
        using iterator_category = std::forward_iterator_tag; //!< Iterator type
        using value_type = DataView::value_type;             //!< Data type
        using difference_type = std::ptrdiff_t;              //!< Difference type
        using pointer = DataView::pointer;                   //!< Pointer to data type
        using reference = DataView::reference;               //!< Reference to data type

        //! Empty constructor
        Iterator();

        //! Construct from view.
        /*!
         * \param view Data view to iterate.
         *
         * This iterator starts at the zero multidimensional index.
         */
        explicit Iterator(const DataView& view);

        //! Construct from view starting at multidimensional index.
        /*!
         * \param view Data view to iterate.
         * \param current Multi-index to start iterator at.
         */
        Iterator(const DataView& view, const int* current);

        //! Dereference iterator.
        reference operator*() const;

        //! Access current data as pointer.
        pointer operator->() const;

        //! Get pointer to current data.
        pointer get() const;

        //! Pre-increment.
        /*!
         * The last dimension of the multidimensional index is incremented by 1.
         */
        Iterator& operator++();

        //! Post-increment.
        /*!
         * The last dimension of the multidimensional index is incremented by 1.
         */
        Iterator operator++(int);

        //! In-place increment.
        /*!
         * \param n Number of elements to increment.
         *
         * The last dimension of the multidimensional index is incremented by n.
         */
        Iterator& operator+=(difference_type n);

        //! Move iterator forward.
        /*!
         * \param n Number of elements to increment.
         *
         * The last dimension of the multidimensional index is incremented by n.
         */
        Iterator operator+(difference_type n);

        //! Check if two iterators are equivalent.
        /*!
         * \return True if both iterators point to the same data.
         *
         * To make the comparison faster, it is assumed both iterators were constructed with the
         * same view.
         */
        bool operator==(const Iterator& other) const;

        //! Check if two iterators are not equivalent.
        /*!
         * \return True if both iterators don't point to the same data.
         */
        bool operator!=(const Iterator& other) const;

        private:
        DataView view_;  //!< View being iterated over
        int current_[N]; //!< Current multidimensional index
        };

    //! Empty constructor.
    DataView();

    //! Constructor.
    /*!
     * \param data Pointer to the data.
     * \param layout Layout of the data.
     */
    DataView(pointer data, const Layout& layout);

    //! Scoped-view constructor.
    /*!
     * \param data Pointer to the data.
     * \param layout Layout of the data.
     * \param start Lower bound of multidimensional indexes within scope.
     * \param shape Shape of multidimensional indexes within scope, beginning with start.
     */
    DataView(pointer data, const Layout& layout, const int* start, const int* shape);

    //! Access an element of the data at a given multidimensional index.
    /*!
     * \param multi_index Multidimensional index to access.
     * \return Data element.
     */
    reference operator()(const int* multi_index) const;

    //! Number of dimensions.
    int num_dimensions() const;

    //! Number of elements per dimension in the scope of the view.
    const int* shape() const;

    //! Number of elements in the scope of the view.
    size_t size() const;

    //! Test if view is not null
    /*!
     * \returns True if data pointer being viewed is not null.
     */
    explicit operator bool() const;

    //! Make an iterator to the start of the data being viewed.
    /*!
     * \return Iterator to start of data.
     */
    Iterator begin() const;

    //! Make and iterator to the end of the data being viewed.
    /*!
     * \return Iterator to end of data.
     */
    Iterator end() const;

    //! Make an iterator to a particular multidimensional index.
    /*!
     * \return Iterator to specified multidimensional index.
     */
    Iterator make_iterator(const int* multi_index) const;

    static int constexpr MAX_NUM_DIMENSIONS = N; //!< Maximum number of dimensions.

    private:
    pointer data_;  //!< Data.
    Layout layout_; //!< Multidimensional array layout.
    int start_[N];  //!< Multi-index for start of scoped view.
    int shape_[N];  //!< Number of elements per dimension of scoped view.
    };

template<typename T, int N>
DataView<T, N>::DataView() : data_(nullptr), layout_(Layout()), start_ {0}, shape_ {0}
    {
    }

template<typename T, int N>
DataView<T, N>::DataView(pointer data, const Layout& layout) : data_(data), layout_(layout)
    {
    assert(data);
    assert(layout.num_dimensions() > 0 && layout.num_dimensions() <= N);

    const auto layout_shape = layout_.shape();
    for (int dim = 0; dim < layout_.num_dimensions(); ++dim)
        {
        start_[dim] = 0;
        shape_[dim] = layout_shape[dim];
        }
    }

template<typename T, int N>
DataView<T, N>::DataView(pointer data, const Layout& layout, const int* start, const int* shape)
    : data_(data), layout_(layout)
    {
    assert(data);
    assert(layout.num_dimensions() > 0 && layout.num_dimensions() <= N);
    assert(start);
    assert(shape);

    for (int dim = 0; dim < layout_.num_dimensions(); ++dim)
        {
        assert(start_[dim] >= 0);
        assert(shape_[dim] > 0);
        assert(start_[dim] + shape_[dim] <= layout.shape()[dim]);
        start_[dim] = start[dim];
        shape_[dim] = shape[dim];
        }
    }

template<typename T, int N>
typename DataView<T, N>::reference DataView<T, N>::operator()(const int* multi_index) const
    {
    assert(multi_index);
    return data_[layout_(multi_index, start_)];
    }

template<typename T, int N>
int DataView<T, N>::num_dimensions() const
    {
    return layout_.num_dimensions();
    }

template<typename T, int N>
const int* DataView<T, N>::shape() const
    {
    return shape_;
    }

template<typename T, int N>
size_t DataView<T, N>::size() const
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

template<typename T, int N>
DataView<T, N>::operator bool() const
    {
    return (data_ != nullptr);
    }

template<typename T, int N>
typename DataView<T, N>::Iterator DataView<T, N>::begin() const
    {
    return Iterator(*this);
    }

template<typename T, int N>
typename DataView<T, N>::Iterator DataView<T, N>::end() const
    {
    return Iterator(*this, shape_);
    }

template<typename T, int N>
typename DataView<T, N>::Iterator DataView<T, N>::make_iterator(const int* multi_index) const
    {
    assert(multi_index);
    return Iterator(*this, multi_index);
    }

template<typename T, int N>
DataView<T, N>::Iterator::Iterator() : view_(DataView()), current_ {0}
    {
    }

template<typename T, int N>
DataView<T, N>::Iterator::Iterator(const DataView& view) : view_(view), current_ {0}
    {
    assert(view);
    }

template<typename T, int N>
DataView<T, N>::Iterator::Iterator(const DataView& view, const int* current) : view_(view)
    {
    assert(view);
    for (int dim = 0; dim < view_.num_dimensions(); ++dim)
        {
        current_[dim] = (current) ? current[dim] : 0;
        }
    }

template<typename T, int N>
typename DataView<T, N>::Iterator::reference DataView<T, N>::Iterator::operator*() const
    {
    return view_(current_);
    }

template<typename T, int N>
typename DataView<T, N>::Iterator::pointer DataView<T, N>::Iterator::operator->() const
    {
    return get();
    }

template<typename T, int N>
typename DataView<T, N>::Iterator::pointer DataView<T, N>::Iterator::get() const
    {
    return &view_(current_);
    }

template<typename T, int N>
typename DataView<T, N>::Iterator& DataView<T, N>::Iterator::operator++()
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

template<typename T, int N>
typename DataView<T, N>::Iterator DataView<T, N>::Iterator::operator++(int)
    {
    Iterator tmp(*this);
    ++(*this);
    return tmp;
    }

template<typename T, int N>
typename DataView<T, N>::Iterator& DataView<T, N>::Iterator::operator+=(difference_type n)
    {
    for (int i = 0; i < n; ++i)
        {
        (*this)++;
        }
    return *this;
    }

template<typename T, int N>
typename DataView<T, N>::Iterator DataView<T, N>::Iterator::operator+(difference_type n)
    {
    Iterator tmp(*this);
    tmp += n;
    return tmp;
    }

template<typename T, int N>
bool DataView<T, N>::Iterator::operator==(const Iterator& other) const
    {
    return (get() == other.get());
    }

template<typename T, int N>
bool DataView<T, N>::Iterator::operator!=(const Iterator& other) const
    {
    return !(*this == other);
    }

    } // namespace flyft

#endif // FLYFT_DATA_VIEW_H_
