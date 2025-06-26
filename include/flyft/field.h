#ifndef FLYFT_FIELD_H_
#define FLYFT_FIELD_H_

#include "flyft/data_layout.h"
#include "flyft/data_view.h"
#include "flyft/tracked_object.h"

#include <algorithm>
#include <complex>

namespace flyft
    {

template<typename T>
class GenericField : public TrackedObject
    {
    public:
    using View = DataView<T>;
    using ConstantView = DataView<const T>;
    using Iterator = typename View::Iterator;
    using ConstantIterator = typename ConstantView::Iterator;

    GenericField() = delete;

    explicit GenericField(char num_dimensions, const int* shape)
        : GenericField(num_dimensions, shape, nullptr)
        {
        }

    GenericField(char num_dimensions, const int* shape, const int* buffer_shape)
        {
        reshape(num_dimensions, shape, buffer_shape);
        }

    // noncopyable / nonmovable
    GenericField(const GenericField&) = delete;
    GenericField(GenericField&&) = delete;
    GenericField& operator=(const GenericField&) = delete;
    GenericField& operator=(GenericField&&) = delete;

    ~GenericField()
        {
        delete[] data_;
        delete[] shape_;
        delete[] buffer_shape_;
        delete[] full_shape_;
        }

    T& operator()(const int* multi_index)
        {
        return view()(multi_index);
        }

    const T& operator()(const int* multi_index) const
        {
        return const_view()(multi_index);
        }

    const int* shape() const
        {
        return shape_;
        }

    const int* buffer_shape() const
        {
        return buffer_shape_;
        }

    const int* full_shape() const
        {
        return layout_.shape();
        }

    View view()
        {
        token_.stageAndCommit();
        return View(data_, layout_, buffer_shape_, shape_);
        }

    ConstantView const_view() const
        {
        return ConstantView(static_cast<const T*>(data_), layout_, buffer_shape_, shape_);
        }

    View full_view()
        {
        token_.stageAndCommit();
        return View(data_, layout_);
        }

    ConstantView const_full_view() const
        {
        return ConstantView(data_, layout_);
        }

    void reshape(char num_dimensions, const int* shape, const int* buffer_shape)
        {
        if (num_dimensions != layout_.num_dimensions())
            {
            delete[] shape_;
            delete[] buffer_shape_;
            delete[] full_shape_;

            shape_ = new int[num_dimensions] {};
            buffer_shape_ = new int[num_dimensions] {};
            full_shape_ = new int[num_dimensions] {};
            }

        bool same_shape = true;
        bool same_buffer_shape = true;
        for (char dim = 0; dim < num_dimensions; ++dim)
            {
            if (shape[dim] != shape_[dim])
                {
                same_shape = false;
                shape_[dim] = shape[dim];
                }

            if (buffer_shape[dim] != buffer_shape_[dim])
                {
                same_buffer_shape = false;
                buffer_shape_[dim] = buffer_shape[dim];
                }

            full_shape_[dim] = shape_[dim] + 2 * buffer_shape_[dim];
            }

        if (!(same_shape && same_buffer_shape))
            {
            DataLayout layout(num_dimensions, full_shape_);

            // if data is alloc'd, decide what to do with it
            if (data_)
                {
                if (same_shape)
                    {
                    // data shape is the same but buffer changed, copy
                    T* tmp = new T[layout.size()];
                    std::fill(tmp, tmp + layout.size(), T(0));

                    // copy the data from old to new
                    auto view_old = const_view();
                    DataView<T> view_new(tmp, layout, buffer_shape, shape);
                    std::copy(view_old.begin(), view_old.end(), view_new.begin());
                    std::swap(data_, tmp);
                    delete[] tmp;
                    }
                else if (layout.size() == layout_.size() && layout.size() > 0)
                    {
                    // total shape is still the same, just reset the data
                    std::fill(data_, data_ + layout.size(), T(0));
                    }
                else
                    {
                    // shape is different, wipe it out and realloc
                    delete[] data_;
                    data_ = nullptr;
                    }
                }

            // if data is still not alloc'd, make it so
            if (!data_ && layout.size() > 0)
                {
                data_ = new T[layout.size()];
                std::fill(data_, data_ + layout.size(), T(0));
                }
            layout_ = layout;
            token_.stageAndCommit();
            }
        }

    void setBuffer(const int* buffer_shape)
        {
        reshape(shape_, buffer_shape);
        }

    void requestBuffer(const int* buffer_shape)
        {
        bool any_increase = false;
        for (char dim = 0; !any_increase && dim < layout_.num_dimensions(); ++dim)
            {
            any_increase |= (buffer_shape[dim] > buffer_shape_[dim]);
            }

        if (any_increase)
            {
            setBuffer(buffer_shape);
            }
        }

    private:
    T* data_;
    int* shape_;
    int* buffer_shape_;
    int* full_shape_;
    DataLayout layout_;
    };

using Field = GenericField<double>;
using ComplexField = GenericField<std::complex<double>>;

    } // namespace flyft

#endif // FLYFT_FIELD_H_
