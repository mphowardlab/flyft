#ifndef FLYFT_FIELD_H_
#define FLYFT_FIELD_H_

#include "flyft/data_layout.h"

#include <algorithm>
#include <complex>
#include <iterator>

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

        template<typename U>
        class Iterator_
            {
            public:
                using iterator_category = std::bidirectional_iterator_tag;
                using difference_type = int;
                using value_type = U;
                using pointer = U*;
                using reference = U&;

                Iterator_()
                    : Iterator_(nullptr,DataLayout(0,0))
                    {}

                Iterator_(value_type* data, const DataLayout& layout)
                    : data_(data), layout_(layout), current_offset_(0)
                    {}

                reference operator()(int offset) const
                    {
                    return data_[layout_(current_offset_+offset)];
                    }

                reference operator[](difference_type n) const
                    {
                    // TODO: add method to layout to handle 1d offsets even if data is not 1d
                    return data_[layout_(current_offset_+n)];
                    }

                reference operator*() const
                    {
                    return *get();
                    }

                pointer operator->() const
                    {
                    return get();
                    }

                pointer get() const
                    {
                    return data_+layout_(current_offset_);
                    }

                Iterator_& operator++()
                    {
                    ++current_offset_;
                    return *this;
                    }

                Iterator_ operator++(int)
                    {
                    Iterator_ tmp(*this);
                    ++current_offset_;
                    return tmp;
                    }

                Iterator_& operator--()
                    {
                    --current_offset_;
                    return *this;
                    }

                Iterator_ operator--(int)
                    {
                    Iterator_ tmp(*this);
                    --current_offset_;
                    return tmp;
                    }

                Iterator_& operator+=(difference_type n)
                    {
                    current_offset_ += n;
                    return *this;
                    }

                Iterator_ operator+(difference_type n) const
                    {
                    Iterator_ tmp(*this);
                    tmp += n;
                    return tmp;
                    }

                friend Iterator_ operator+(difference_type n, const Iterator_ self)
                    {
                    return self+n;
                    }

                Iterator_& operator-=(difference_type n)
                    {
                    current_offset_ -= n;
                    return *this;
                    }

                Iterator_ operator-(difference_type n) const
                    {
                    Iterator_ tmp(*this);
                    tmp -= n;
                    return tmp;
                    }

                difference_type operator-(const Iterator_ other) const
                    {
                    // TODO: add method to layout to figure out distance between two elements
                    return (current_offset_-other.current_offset_);
                    }

                operator bool() const
                    {
                    return data_ != nullptr;
                    }

                bool operator==(const Iterator_& other) const
                    {
                    return (get() == other.get());
                    }

                bool operator!=(const Iterator_& other) const
                    {
                    return !(*this == other);
                    }

            private:
                value_type* data_;
                DataLayout layout_;
                int current_offset_;
            };
        using iterator = Iterator_<T>;
        using const_iterator = Iterator_<const T>;

        T& operator()(int idx)
            {
            return data_[layout_(idx)];
            }

        const T& operator()(int idx) const
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

        iterator begin()
            {
            return iterator(data_,layout_);
            }

        iterator end()
            {
            return begin()+layout_.shape();
            }

        iterator begin_full()
            {
            return iterator(data_,layout_.withoutBuffer());
            }

        iterator end_full()
            {
            return begin_full()+layout_.full_shape();
            }

        const_iterator cbegin() const
            {
            return const_iterator(static_cast<const T*>(data_),layout_);
            }

        const_iterator cend() const
            {
            return cbegin()+layout_.shape();
            }

        const_iterator cbegin_full()
            {
            return const_iterator(static_cast<const T*>(data_),layout_.withoutBuffer());
            }

        const_iterator cend_full()
            {
            return cbegin_full()+layout_.full_shape();
            }

        void reshape(int shape, int buffer_shape)
            {
            reshape(DataLayout(shape,buffer_shape));
            }

        void reshape(const DataLayout& layout)
            {
            if (layout != layout_)
                {
                // if data is alloc'd, decide what to do with it
                if (data_ != nullptr)
                    {
                    if (layout.shape() == layout_.shape())
                        {
                        // data shape is the same but buffer changed, copy
                        T* tmp = new T[layout.full_shape()];
                        std::fill(tmp,tmp+layout.full_shape(),T(0));
                        for (int i=0; i < layout.shape(); ++i)
                            {
                            tmp[layout(i)] = data_[layout_(i)];
                            }
                        std::swap(data_,tmp);
                        delete[] tmp;
                        }
                    else if (layout.full_shape() == layout_.full_shape() && layout.full_shape() > 0)
                        {
                        // total shape is still the same, just reset the data
                        std::fill(data_,data_+layout.full_shape(),T(0));
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
