#include "flyft/communicator.h"

namespace flyft
    {

Communicator::Communicator()
#ifdef FLYFT_MPI
    : Communicator(MPI_COMM_WORLD, 0)
#else
    : size_(1), rank_(0), root_(0)
#endif // FLYFT_MPI
    {
    }

#ifdef FLYFT_MPI
Communicator::Communicator(MPI_Comm comm, int root) : comm_(comm), root_(root)
    {
    MPI_Comm_size(comm_, &size_);
    MPI_Comm_rank(comm_, &rank_);
    }
#endif // FLYFT_MPI

Communicator::~Communicator() {}

#ifdef FLYFT_MPI
MPI_Comm Communicator::get() const
    {
    return comm_;
    }
#endif // FLYFT_MPI

int Communicator::size() const
    {
    return size_;
    }

int Communicator::rank() const
    {
    return rank_;
    }

int Communicator::root() const
    {
    return root_;
    }

bool Communicator::any(bool flag) const
    {
    bool value = flag;
#ifdef FLYFT_MPI
    if (size_ > 1)
        {
        MPI_Allreduce(MPI_IN_PLACE, &value, 1, MPI_CXX_BOOL, MPI_LOR, comm_);
        }
#endif // FLYFT_MPI
    return value;
    }

bool Communicator::all(bool flag) const
    {
    bool value = flag;
#ifdef FLYFT_MPI
    if (size_ > 1)
        {
        MPI_Allreduce(MPI_IN_PLACE, &value, 1, MPI_CXX_BOOL, MPI_LAND, comm_);
        }
#endif // FLYFT_MPI
    return value;
    }

    } // namespace flyft
