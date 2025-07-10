#ifndef FLYFT_DATA_ARRAY_H_
#define FLYFT_DATA_ARRAY_H_

#include "flyft/data_layout.h"
#include "flyft/data_view.h"
#include "flyft/tracked_object.h"

#include <algorithm>

namespace flyft
    {

//! Multidimensional array.
/*!
 * A DataArray is a multidimensional array. It allocates and stores its own data consistent with a
 * certain number of dimensions and shape, as defined by a DataLayout.
 *
 * The \b full shape of the DataArray includes two sub-shapes: the shape and the buffer shape. The
 * full shape is the shape plus one buffer shape on both sides. The intention is that the shape be
 * used for locally owned data and the buffers be used for data owned by other processors when
 * arrays are broken across processors in parallel simulations.
 *
 * A new buffer shape can be requested, and the buffer shape along each dimension is the largest
 * value requested.
 *
 * A DataArray can be reshaped. If the buffer shape is changed but the shape is not, the data stored
 * in the shape will be copied.
 *
 * A DataArray is a TrackedObject. This means that potential modifications to its values are tracked
 * when a view of the data is taken. For performance elsewhere, it may be important to use
 * const_view when the data is only being read.
 *
 * The maximum dimension that can be represented by the layout is set by the template parameter
 * \a N.
 */
template<typename T, int N>
class DataArray : public TrackedObject
    {
    public:
    using View = DataView<T, N>;
    using ConstantView = DataView<const T, N>;
    using Iterator = typename View::Iterator;
    using ConstantIterator = typename ConstantView::Iterator;

    // no default constructor
    DataArray() = delete;

    //! Constructor.
    /*!
     * \param num_dimensions Number of dimensions.
     * \param shape Number of elements per dimension.
     */
    DataArray(int num_dimensions, const int* shape);

    //! Constructor with buffer.
    /*!
     * \param num_dimensions Number of dimensions.
     * \param shape Number of elements per dimension.
     * \param buffer_shape Number of elements in buffers per dimension.
     */
    DataArray(int num_dimensions, const int* shape, const int* buffer_shape);

    // noncopyable / nonmovable
    DataArray(const DataArray&) = delete;
    DataArray(DataArray&&) = delete;
    DataArray& operator=(const DataArray&) = delete;
    DataArray& operator=(DataArray&&) = delete;

    //! Destructor.
    ~DataArray();

    //! Access an element.
    /*!
     * \param multi_index Multidimensional index.
     * \return Element.
     *
     * Accessing this way allows the element to be modified, so it will advance the tracked state
     * of the array.
     */
    T& operator()(const int* multi_index);

    //! Access an element.
    /*!
     * \param multi_index Multidimensional index.
     * \return Element.
     */
    const T& operator()(const int* multi_index) const;

    //! Number of elements per dimension.
    const int* shape() const;

    //! Number of elements in each buffer per dimension.
    const int* buffer_shape() const;

    //! Total number of elements per dimension.
    /*!
     * If there are $S$ elements in the shape and $B$ elements in the buffer for a dimension, then
     * the total number of elements for the dimension is $F = S + 2B$.
     */
    const int* full_shape() const;

    //! View of elements in the shape.
    /*!
     * This view allows the data array to be modified, so the tracked state of the array will be
     * advanced when it is created.
     */
    View view();

    //! Read-only view of elements in the shape.
    ConstantView const_view() const;

    //! View of elements in the full shape.
    /*!
     * This view allows the data array to be modified, so the tracked state of the array will be
     * advanced when it is created.
     */
    View full_view();

    //! Read-only view of elements in the full shape.
    ConstantView const_full_view() const;

    //! Reshape the data array.
    /*!
     * \param num_dimensions Number of dimensions.
     * \param shape Number of elements per dimension.
     * \param buffer_shape Number of buffer elements per dimension.
     *
     * If the shape stays the same but the buffer shape changes, the data in the shape will be
     * copied to the new array. Otherwise, all data will be destroyed.
     */
    void reshape(int num_dimensions, const int* shape, const int* buffer_shape);

    //! Force a new buffer shape.
    /*!
     * \param buffer_shape Number of buffer elements per dimension.
     *
     * The buffer shape is immediately resized.
     */
    void setBuffer(const int* buffer_shape);

    //! Request a new buffer shape.
    /*!
     * \param buffer_shape Number of buffer elements per dimension.
     *
     * The buffer shape only changes if it increases in at least one dimension.
     */
    void requestBuffer(const int* buffer_shape);

    private:
    T* data_;                      //!< Data.
    int shape_[N];                 //!< Number of elements per dimension.
    int buffer_shape_[N];          //!< Number of elements in buffer per dimension.
    typename View::Layout layout_; //!< Layout of the data array.
    };

template<typename T, int N>
DataArray<T, N>::DataArray(int num_dimensions, const int* shape)
    : DataArray(num_dimensions, shape, nullptr)
    {
    }

template<typename T, int N>
DataArray<T, N>::DataArray(int num_dimensions, const int* shape, const int* buffer_shape)
    : shape_ {0}, buffer_shape_ {0}
    {
    assert(num_dimensions > 0 && num_dimensions <= N);
    assert(shape);
    reshape(num_dimensions, shape, buffer_shape);
    }

template<typename T, int N>
DataArray<T, N>::~DataArray()
    {
    free(data_);
    data_ = nullptr;
    }

template<typename T, int N>
T& DataArray<T, N>::operator()(const int* multi_index)
    {
    assert(multi_index);
    return view()(multi_index);
    }

template<typename T, int N>
const T& DataArray<T, N>::operator()(const int* multi_index) const
    {
    assert(multi_index);
    return const_view()(multi_index);
    }

template<typename T, int N>
const int* DataArray<T, N>::shape() const
    {
    return shape_;
    }

template<typename T, int N>
const int* DataArray<T, N>::buffer_shape() const
    {
    return buffer_shape_;
    }

template<typename T, int N>
const int* DataArray<T, N>::full_shape() const
    {
    return layout_.shape();
    }

template<typename T, int N>
typename DataArray<T, N>::View DataArray<T, N>::view()
    {
    token_.stageAndCommit();
    return View(data_, layout_, buffer_shape_, shape_);
    }

template<typename T, int N>
typename DataArray<T, N>::ConstantView DataArray<T, N>::const_view() const
    {
    return ConstantView(static_cast<const T*>(data_), layout_, buffer_shape_, shape_);
    }

template<typename T, int N>
typename DataArray<T, N>::View DataArray<T, N>::full_view()
    {
    token_.stageAndCommit();
    return View(data_, layout_);
    }

template<typename T, int N>
typename DataArray<T, N>::ConstantView DataArray<T, N>::const_full_view() const
    {
    return ConstantView(data_, layout_);
    }

template<typename T, int N>
void DataArray<T, N>::reshape(int num_dimensions, const int* shape, const int* buffer_shape)
    {
    assert(num_dimensions > 0 && num_dimensions <= N);

    bool same_shape = true;
    bool same_buffer_shape = true;
    std::vector<int> full_shape(num_dimensions);
    for (int dim = 0; dim < num_dimensions; ++dim)
        {
        if (shape && shape[dim] != shape_[dim])
            {
            same_shape = false;
            shape_[dim] = shape[dim];
            assert(shape_[dim] > 0);
            }

        if (buffer_shape && buffer_shape[dim] != buffer_shape_[dim])
            {
            same_buffer_shape = false;
            buffer_shape_[dim] = buffer_shape[dim];
            assert(buffer_shape_[dim] >= 0);
            }

        full_shape[dim] = shape_[dim] + 2 * buffer_shape_[dim];
        }

    if (!(same_shape && same_buffer_shape))
        {
        typename View::Layout layout(num_dimensions, full_shape.data());

        // if data is alloc'd, decide what to do with it
        if (data_)
            {
            if (same_shape)
                {
                // data shape is the same but buffer changed, copy
                T* tmp = static_cast<T*>(malloc(layout.size() * sizeof(T)));
                std::fill(tmp, tmp + layout.size(), T(0));

                // copy the data from old to new
                auto view_old = const_view();
                View view_new(tmp, layout, buffer_shape, shape);
                std::copy(view_old.begin(), view_old.end(), view_new.begin());
                std::swap(data_, tmp);
                free(tmp);
                }
            else if (layout.size() == layout_.size() && layout.size() > 0)
                {
                // total shape is still the same, just reset the data
                std::fill(data_, data_ + layout.size(), T(0));
                }
            else
                {
                // shape is different, wipe it out and realloc
                free(data_);
                data_ = nullptr;
                }
            }

        // if data is still not alloc'd, make it so
        if (!data_ && layout.size() > 0)
            {
            data_ = static_cast<T*>(malloc(layout.size() * sizeof(T)));
            std::fill(data_, data_ + layout.size(), T(0));
            }
        layout_ = layout;
        token_.stageAndCommit();
        }
    }

template<typename T, int N>
void DataArray<T, N>::setBuffer(const int* buffer_shape)
    {
    reshape(shape_, buffer_shape);
    }

template<typename T, int N>
void DataArray<T, N>::requestBuffer(const int* buffer_shape)
    {
    // don't do anything if buffer shape is null
    if (!buffer_shape)
        return;

    bool any_increase = false;
    for (int dim = 0; !any_increase && dim < layout_.num_dimensions(); ++dim)
        {
        if (buffer_shape[dim] > buffer_shape_[dim])
            {
            any_increase = true;
            }
        }

    if (any_increase)
        {
        setBuffer(buffer_shape);
        }
    }

    } // namespace flyft

#endif // FLYFT_DATA_ARRAY_H_
