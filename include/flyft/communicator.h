#ifndef FLYFT_COMMUNICATOR_H_
#define FLYFT_COMMUNICATOR_H_

#ifdef FLYFT_MPI
#include <mpi.h>
#endif

namespace flyft
{

class Communicator
    {
    public:
        Communicator();
        #ifdef FLYFT_MPI
        Communicator(MPI_Comm comm);
        #endif

        ~Communicator();

        #ifdef FLYFT_MPI
        MPI_Comm get() const;  
        #endif
        int size() const;
        int rank() const;

    private:
        #ifdef FLYFT_MPI
        MPI_Comm comm_;
        #endif
        int size_;
        int rank_;
    };

}

#endif // FLYFT_COMMUNICATOR_H_
