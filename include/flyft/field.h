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
            : data_(nullptr), shape_(-1)
            {
            reshape(shape);
            }

        // noncopyable / nonmovable
        GenericField(const GenericField&) = delete;
        GenericField(GenericField&&) = delete;
        GenericField& operator=(const GenericField&) = delete;
        GenericField& operator=(GenericField&&) = delete;

        ~GenericField()
            {
            if (data_) delete data_;
            }

        T* data()
            {
            return data_;
            }

        const T* data() const
            {
            return static_cast<const T*>(data_);
            }

        int shape() const
            {
            return shape_;
            }

        void reshape(int shape)
            {
            if (shape != shape_)
                {
                if (data_ != nullptr) delete data_;
                data_ = new T[shape];
                std::fill(data_, data_+shape, T(0));
                }
            shape_ = shape;
            }

    private:
        T* data_;
        int shape_;
    };

typedef GenericField<double> Field;
typedef GenericField<std::complex<double>> ComplexField;

}

#endif // FLYFT_FIELD_H_
