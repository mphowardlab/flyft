#include "flyft/communicator.h"

#include <stdexcept>

namespace flyft
{

Communicator::Communicator()
#ifdef FLYFT_MPI
    : Communicator(MPI_COMM_WORLD)
#else
    : size_(1), rank_(0)
#endif // FLYFT_MPI
    {
    }

#ifdef FLYFT_MPI
Communicator::Communicator(MPI_Comm comm)
    : comm_(comm)
    {
    MPI_Comm_size(comm_,&size_);
    MPI_Comm_rank(comm_,&rank_);
    }
#endif // FLYFT_MPI

Communicator::~Communicator()
    {
    }

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

void Communicator::sync(std::shared_ptr<Field> field)
    {
    // check if field was recently synced and stop if not needed
    auto token = field_tokens_.find(field->id());
    if (token != field_tokens_.end() && token->second == field->token())
        {
        return;
        }

    // check field shape
    const int shape = field->shape();
    const int buffer_shape = field->buffer_shape();
    if (buffer_shape > shape)
        {
        // ERROR: overdecomposed (only nearest-neighbor comms supported)
        throw std::runtime_error("Mesh overdecomposed");
        }
    else if (buffer_shape == 0)
        {
        // nothing to do, no buffer needed
        return;
        }

    // sync field
    const auto f = field->view();
    #ifdef FLYFT_MPI
    if (size_ > 1)
        {
        const int left = (rank_ != 0) ? rank_-1 : size_-1;
        const int right = (rank_ != size_-1) ? rank_+1 : 0;

        MPI_Request requests[4];
        // receive left buffer from left (tag 0), right buffer from right (tag 1)
        MPI_Irecv(&f(-buffer_shape),buffer_shape,MPI_DOUBLE,left,0,comm_,&requests[0]);
        MPI_Irecv(&f(shape),buffer_shape,MPI_DOUBLE,right,1,comm_,&requests[1]);
        // send left edge to left (tag 1), right edge to right (tag 0)
        MPI_Isend(&f(0),buffer_shape,MPI_DOUBLE,left,1,comm_,&requests[2]);
        MPI_Isend(&f(shape-buffer_shape),buffer_shape,MPI_DOUBLE,right,0,comm_,&requests[3]);
        // wait for communication to finish
        MPI_Waitall(4,requests,MPI_STATUSES_IGNORE);
        }
    else
    #endif
        {
        for (int idx=0; idx < buffer_shape; ++idx)
            {
            f(shape+idx) = f(idx);
            f(-1-idx) = f(shape-1-idx);
            }
        }

    // cache token
    field_tokens_[field->id()] = field->token();
    }

bool Communicator::any(bool flag) const
    {
    bool value = flag;
    #ifdef FLYFT_MPI
    if (size_ > 1)
        {
        MPI_Allreduce(MPI_IN_PLACE,&value,1,MPI_CXX_BOOL,MPI_LOR,comm_);
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
        MPI_Allreduce(MPI_IN_PLACE,&value,1,MPI_CXX_BOOL,MPI_LAND,comm_);
        }
    #endif // FLYFT_MPI
    return value;
    }

}
