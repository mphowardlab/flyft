#include "flyft/communicator.h"

namespace flyft
{

Communicator::Communicator()
#ifdef FLYFT_MPI
    : Communicator(MPI_COMM_WORLD)
#else
    : size_(1), rank_(0)
#endif
    {
    }

#ifdef FLYFT_MPI
Communicator::Communicator(MPI_Comm comm)
    : comm_(comm)
    {
    MPI_Comm_size(comm_,&size_);
    MPI_Comm_rank(comm_,&rank_);
    }
#endif

Communicator::~Communicator()
    {
    }

#ifdef FLYFT_MPI
MPI_Comm Communicator::get() const
    {
    return comm_;
    }        
#endif

int Communicator::size() const
    {
    return size_;
    }

int Communicator::rank() const
    {
    return rank_;
    }

}
