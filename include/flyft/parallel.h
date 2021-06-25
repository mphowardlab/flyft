#ifndef FLYFT_PARALLEL_H_
#define FLYFT_PARALLEL_H_

#include <cstddef>

namespace flyft
{
namespace parallel
{

template<typename T, typename U>
U accumulate(const T* src, size_t size, const U& value)
    {
    U sum = value;
    #ifdef FLYFT_OPENMP
    #pragma omp parallel for schedule(static) default(none) shared(src,size,value) reduction(+:sum)
    #endif
    for (size_t idx=0; idx < size; ++idx)
        {
        sum += src[idx];
        }
    return sum;
    }

template<typename T, typename U>
void copy(const T* src, size_t size, U* dest)
    {
    #ifdef FLYFT_OPENMP
    #pragma omp parallel for schedule(static) default(none) shared(src,size,dest)
    #endif
    for (size_t idx=0; idx < size; ++idx)
        {
        dest[idx] = src[idx];
        }
    }

template<typename T, typename U>
void fill(T* dest, size_t size, const U& value)
    {
    #ifdef FLYFT_OPENMP
    #pragma omp parallel for schedule(static) default(none) shared(dest,size,value)
    #endif
    for (size_t idx=0; idx < size; ++idx)
        {
        dest[idx] = value;
        }
    }

template<typename T>
class ScaleOperation
    {
    public:
        template<typename U>
        ScaleOperation(const U& value)
            : value_(value)
            {
            }

        template<typename U>
        auto operator()(const U& x) const
            {
            return x/value_;
            }

    private:
        T value_;
    };

template<typename T, typename U, class UnaryOperation>
void transform(T* input, size_t size, U* output, const UnaryOperation& unary_op)
    {
    #ifdef FLYFT_OPENMP
    #pragma omp parallel for schedule(static) default(none) shared(input,size,output,unary_op)
    #endif
    for (size_t idx=0; idx < size; ++idx)
        {
        output[idx] = unary_op(input[idx]);
        }
    }

}
}

#endif // FLYFT_PARALLEL_H_
