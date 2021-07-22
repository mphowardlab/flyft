#ifndef FLYFT_FIELD_H_
#define FLYFT_FIELD_H_

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
            {
            }
        GenericField(int shape, int buffer_shape)
            : data_(nullptr), shape_(-1), buffer_shape_(-1)
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

        double& operator()(int idx)
            {
            return data_[idx];
            }

        double operator()(int idx) const
            {
            return data_[idx];
            }

        int shape() const
            {
            return shape_;
            }

        int buffer_shape() const
            {
            return buffer_shape_;
            }

        int buffered_shape() const
            {
            return shape_ + 2*buffer_shape_;
            }

        T* begin()
            {
            return data_;
            }

        const T* begin() const
            {
            return static_cast<const T*>(data_);
            }

        T* end()
            {
            return begin() + buffered_shape();
            }

        const T* end() const
            {
            return begin() + buffered_shape();
            }

        T* first()
            {
            return begin() + buffer_shape_;
            }

        const T* first() const
            {
            return begin() + buffer_shape_;
            }

        T* last()
            {
            return first() + shape_;
            }

        const T* last() const
            {
            return first() + shape_;
            }

        void reshape(int shape, int buffer_shape=0)
            {
            if (shape != this->shape() || buffer_shape != this->buffer_shape())
                {
                int buffered_shape = shape + 2*buffer_shape;
                // if data is alloc'd, decide what to do with it
                if (data_ != nullptr)
                    {
                    if (shape == this->shape())
                        {
                        // data shape is the same but buffer changed, copy
                        T* tmp = new T[buffered_shape];
                        for (int i=0; i < buffered_shape; ++i)
                            {
                            int offset = (i-buffer_shape);
                            if (offset >= 0 && offset < shape)
                                {
                                tmp[i] = this->first()[offset];
                                }
                            else
                                {
                                tmp[i] = T(0);
                                }
                            }
                        std::swap(data_,tmp);
                        delete[] tmp;
                        }
                    else if (buffered_shape == this->buffered_shape())
                        {
                        // total shape is still the same, just reset the data
                        std::fill(data_,data_+buffered_shape,T(0));
                        }
                    else
                        {
                        // shape is different, wipe it out and realloc
                        delete data_;
                        }
                    }

                // if data is still not alloc'd, make it so
                if (data_ == nullptr)
                    {
                    data_ = new T[buffered_shape];
                    std::fill(data_,data_+buffered_shape,T(0));
                    }

                // set the new shape of the array & buffer
                shape_ = shape;
                buffer_shape_ = buffer_shape;
                }
            }

    private:
        T* data_;
        int shape_;
        int buffer_shape_;
    };

using Field = GenericField<double>;
using ComplexField = GenericField<std::complex<double>>;

}

#endif // FLYFT_FIELD_H_
