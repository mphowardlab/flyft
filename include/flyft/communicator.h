#ifndef FLYFT_COMMUNICATOR_H_
#define FLYFT_COMMUNICATOR_H_

#include "flyft/tracked_object.h"

#ifdef FLYFT_MPI
#include <mpi.h>
#endif // FLYFT_MPI

#include <complex>
#include <cstdint>
#include <type_traits>

namespace flyft
{

class Communicator : public TrackedObject
    {
    public:
        Communicator();
        #ifdef FLYFT_MPI
        Communicator(MPI_Comm comm, int root);
        #endif

        ~Communicator();

        #ifdef FLYFT_MPI
        MPI_Comm get() const;  
        #endif
        int size() const;
        int rank() const;
        int root() const;

        bool any(bool flag) const;
        bool all(bool flag) const;

        template<typename T>
        T max(T value) const
            {
            T tmp = value;
            #ifdef FLYFT_MPI
            if (size_ > 1)
                {
                MPI_Allreduce(&value,&tmp,1,mpi_type<T>(),MPI_MAX,comm_);
                }
            #endif // FLYFT_MPI
            return tmp;
            }

        template<typename T>
        T min(T value) const
            {
            T tmp = value;
            #ifdef FLYFT_MPI
            if (size_ > 1)
                {
                MPI_Allreduce(&value,&tmp,1,mpi_type<T>(),MPI_MIN,comm_);
                }
            #endif // FLYFT_MPI
            return tmp;
            }

        template<typename T>
        T sum(T value) const
            {
            T tmp = value;
            #ifdef FLYFT_MPI
            if (size_ > 1)
                {
                MPI_Allreduce(&value,&tmp,1,mpi_type<T>(),MPI_SUM,comm_);
                }
            #endif // FLYFT_MPI
            return tmp;
            }

    private:
        #ifdef FLYFT_MPI
        MPI_Comm comm_;
        #endif
        int size_;
        int rank_;
        int root_;

        #ifdef FLYFT_MPI
        // https://gist.github.com/2b-t/50d85115db8b12ed263f8231abf07fa2
        template<typename T>
        constexpr MPI_Datatype mpi_type() const noexcept
            {
            MPI_Datatype type = MPI_DATATYPE_NULL;
            if (std::is_same<T,char>::value)
                {
                type = MPI_CHAR;
                }
            else if (std::is_same<T,signed char>::value)
                {
                type = MPI_SIGNED_CHAR;
                }
            else if (std::is_same<T,unsigned char>::value)
                {
                type = MPI_UNSIGNED_CHAR;
                }
            else if (std::is_same<T,wchar_t>::value)
                {
                type = MPI_WCHAR;
                }
            else if (std::is_same<T,signed short>::value)
                {
                type = MPI_SHORT;
                }
            else if (std::is_same<T,unsigned short>::value)
                {
                type = MPI_UNSIGNED_SHORT;
                }
            else if (std::is_same<T,signed int>::value)
                {
                type = MPI_INT;
                }
            else if (std::is_same<T,unsigned int>::value)
                {
                type = MPI_UNSIGNED;
                }
            else if (std::is_same<T,signed long int>::value)
                {
                type = MPI_LONG;
                }
            else if (std::is_same<T,unsigned long int>::value)
                {
                type = MPI_UNSIGNED_LONG;
                }
            else if (std::is_same<T,signed long long int>::value)
                {
                type = MPI_LONG_LONG;
                }
            else if (std::is_same<T,unsigned long long int>::value)
                {
                type = MPI_UNSIGNED_LONG_LONG;
                }
            else if (std::is_same<T,float>::value)
                {
                type = MPI_FLOAT;
                }
            else if (std::is_same<T,double>::value)
                {
                type = MPI_DOUBLE;
                }
            else if (std::is_same<T,long double>::value)
                {
                type = MPI_LONG_DOUBLE;
                }
            else if (std::is_same<T,std::int8_t>::value)
                {
                type = MPI_INT8_T;
                }
            else if (std::is_same<T,std::int16_t>::value)
                {
                type = MPI_INT16_T;
                }
            else if (std::is_same<T,std::int32_t>::value)
                {
                type = MPI_INT32_T;
                }
            else if (std::is_same<T,std::int64_t>::value)
                {
                type = MPI_INT64_T;
                }
            else if (std::is_same<T,std::uint8_t>::value)
                {
                type = MPI_UINT8_T;
                }
            else if (std::is_same<T,std::uint16_t>::value)
                {
                type = MPI_UINT16_T;
                }
            else if (std::is_same<T,std::uint32_t>::value)
                {
                type = MPI_UINT32_T;
                }
            else if (std::is_same<T,std::uint64_t>::value)
                {
                type = MPI_UINT64_T;
                }
            else if (std::is_same<T,bool>::value)
                {
                type = MPI_CXX_BOOL;
                }
            else if (std::is_same<T,std::complex<float>>::value)
                {
                type = MPI_CXX_FLOAT_COMPLEX;
                }
            else if (std::is_same<T,std::complex<double>>::value)
                {
                type = MPI_CXX_DOUBLE_COMPLEX;
                }
            else if (std::is_same<T,std::complex<long double>>::value)
                {
                type = MPI_CXX_LONG_DOUBLE_COMPLEX;
                }
            return type;
            }
        #endif // FLYFT_MPI
    };

}

#endif // FLYFT_COMMUNICATOR_H_
