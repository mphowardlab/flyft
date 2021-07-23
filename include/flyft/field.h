#ifndef FLYFT_FIELD_H_
#define FLYFT_FIELD_H_

#include "flyft/data_layout.h"

#include <algorithm>
#include <complex>

namespace flyft
{

template<typename T>
class GenericField
    {
    public:
        GenericField() = delete;
        GenericField(int shape)
            : GenericField(shape,0)
            {}
        GenericField(int shape, int buffer_shape)
            : GenericField(DataLayout(shape,buffer_shape))
            {}
        GenericField(const DataLayout& layout)
            : data_(nullptr)
            {
            reshape(layout);
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

        double& operator()(int idx)
            {
            return data_[layout_(idx)];
            }

        double operator()(int idx) const
            {
            return data_[layout_(idx)];
            }

        int shape() const
            {
            return layout_.shape();
            }

        int buffer_shape() const
            {
            return layout_.buffer_shape();
            }

        int full_shape() const
            {
            return layout_.full_shape();
            }

        T* begin()
            {
            return data_;
            }

        const T* cbegin() const
            {
            return static_cast<const T*>(data_);
            }

        T* end()
            {
            return begin() + full_shape();
            }

        const T* cend() const
            {
            return cbegin() + full_shape();
            }

        void reshape(int shape, int buffer_shape)
            {
            reshape(DataLayout(shape,buffer_shape));
            }

        void reshape(const DataLayout& layout)
            {
            if (layout != layout_)
                {
                int full_shape = layout.full_shape();
                // if data is alloc'd, decide what to do with it
                if (data_ != nullptr)
                    {
                    if (layout.shape() == layout_.shape())
                        {
                        // data shape is the same but buffer changed, copy
                        T* tmp = new T[full_shape];
                        for (int i=0; i < full_shape; ++i)
                            {
                            int offset = layout(i);
                            if (offset >= 0 && offset < layout.shape())
                                {
                                tmp[i] = data_[layout_(offset)];
                                }
                            else
                                {
                                tmp[i] = T(0);
                                }
                            }
                        std::swap(data_,tmp);
                        delete[] tmp;
                        }
                    else if (full_shape == layout_.full_shape() && full_shape > 0)
                        {
                        // total shape is still the same, just reset the data
                        std::fill(data_,data_+full_shape,T(0));
                        }
                    else
                        {
                        // shape is different, wipe it out and realloc
                        delete[] data_;
                        data_ = nullptr;
                        }
                    }

                // if data is still not alloc'd, make it so
                if (data_ == nullptr && layout.full_shape() > 0)
                    {
                    data_ = new T[layout.full_shape()];
                    std::fill(data_,data_+layout.full_shape(),T(0));
                    }
                layout_ = layout;
                }
            }

    private:
        T* data_;
        DataLayout layout_;
    };

using Field = GenericField<double>;
using ComplexField = GenericField<std::complex<double>>;

}

#endif // FLYFT_FIELD_H_
