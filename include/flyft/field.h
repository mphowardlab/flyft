#ifndef FLYFT_FIELD_H_
#define FLYFT_FIELD_H_

#include "flyft/data_layout.h"
#include "flyft/data_token.h"
#include "flyft/data_view.h"

#include <algorithm>
#include <complex>

namespace flyft
{

template<typename T>
class GenericField
    {
    public:
        GenericField() = delete;
        explicit GenericField(int shape)
            : GenericField(shape,0)
            {}
        GenericField(int shape, int buffer_shape)
            : id_(count++), token_(id_), data_(nullptr), shape_(0), buffer_shape_(0), layout_(0)
            {
            reshape(shape,buffer_shape);
            }

        // noncopyable / nonmovable
        GenericField(const GenericField&) = delete;
        GenericField(GenericField&&) = delete;
        GenericField& operator=(const GenericField&) = delete;
        GenericField& operator=(GenericField&&) = delete;

        ~GenericField()
            {
            if (data_) delete[] data_;
            }

        using View = DataView<T>;
        using ConstantView = DataView<const T>;
        using Iterator = typename View::Iterator;
        using ConstantIterator = typename ConstantView::Iterator;
        using Identifier = typename DataToken::Type;

        T& operator()(int idx)
            {
            return view()(idx);
            }

        const T& operator()(int idx) const
            {
            return const_view()(idx);
            }

        int shape() const
            {
            return shape_;
            }

        int buffer_shape() const
            {
            return buffer_shape_;
            }

        int full_shape() const
            {
            return shape_+2*buffer_shape_;
            }

        Identifier id() const
            {
            return id_;
            }

        const DataToken& token() const
            {
            return token_;
            }

        View view()
            {
            token_.advance();
            return View(data_,layout_,buffer_shape_,full_shape()-buffer_shape_);
            }

        ConstantView const_view() const
            {
            return ConstantView(static_cast<const T*>(data_),layout_,buffer_shape_,full_shape()-buffer_shape_);
            }

        View full_view()
            {
            token_.advance();
            return View(data_,layout_,0,full_shape());
            }

        ConstantView const_full_view() const
            {
            return ConstantView(data_,layout_,0,full_shape());
            }

        void reshape(int shape, int buffer_shape)
            {
            if (shape != shape_ || buffer_shape != buffer_shape_)
                {
                DataLayout layout(shape+2*buffer_shape);

                // if data is alloc'd, decide what to do with it
                if (data_ != nullptr)
                    {
                    if (shape == shape_)
                        {
                        // data shape is the same but buffer changed, copy
                        T* tmp = new T[layout.size()];
                        std::fill(tmp,tmp+layout.size(),T(0));

                        // copy the data from old to new
                        auto view_old = const_view();
                        DataView<T> view_new(tmp,layout,buffer_shape,buffer_shape+shape);
                        std::copy(view_old.begin(),view_old.end(),view_new.begin());
                        std::swap(data_,tmp);
                        delete[] tmp;
                        }
                    else if (layout.size() == layout_.size() && layout.size() > 0)
                        {
                        // total shape is still the same, just reset the data
                        std::fill(data_,data_+layout.size(),T(0));
                        }
                    else
                        {
                        // shape is different, wipe it out and realloc
                        delete[] data_;
                        data_ = nullptr;
                        }
                    }

                // if data is still not alloc'd, make it so
                if (data_ == nullptr && layout.size() > 0)
                    {
                    data_ = new T[layout.size()];
                    std::fill(data_,data_+layout.size(),T(0));
                    }
                shape_ = shape;
                buffer_shape_ = buffer_shape;
                layout_ = layout;
                token_.advance();
                }
            }

        void setBuffer(int buffer_shape)
            {
            reshape(shape_,buffer_shape);
            }

        void requestBuffer(int buffer_shape)
            {
            if (buffer_shape > buffer_shape_)
                {
                setBuffer(buffer_shape);
                }
            }

    private:
        Identifier id_;
        DataToken token_;
        T* data_;
        int shape_;
        int buffer_shape_;
        DataLayout layout_;

        static Identifier count;
    };
template<typename T>
typename GenericField<T>::Identifier GenericField<T>::count = 0;

using Field = GenericField<double>;
using ComplexField = GenericField<std::complex<double>>;

}

#endif // FLYFT_FIELD_H_
