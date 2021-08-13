#include "flyft/parallel_mesh.h"

#include <stdexcept>

namespace flyft
{

ParallelMesh::ParallelMesh(std::shared_ptr<Mesh> mesh, std::shared_ptr<Communicator> comm)
    {
    setup(mesh,comm);
    }

ParallelMesh::~ParallelMesh()
    {
    }

std::shared_ptr<Communicator> ParallelMesh::getCommunicator()
    {
    return comm_;
    }

std::shared_ptr<const Communicator> ParallelMesh::getCommunicator() const
    {
    return comm_;
    }

std::shared_ptr<const Mesh> ParallelMesh::local() const
    {
    return local_mesh_;
    }

std::shared_ptr<const Mesh> ParallelMesh::full() const
    {
    return full_mesh_;
    }

int ParallelMesh::getProcessorShape() const
    {
    return layout_.shape();
    }

int ParallelMesh::getProcessorCoordinates() const
    {
    return coords_;
    }

int ParallelMesh::getProcessorCoordinatesByOffset(int offset) const
    {
    int proc = coords_ + offset;
    while (proc < 0)
        {
        proc += layout_.shape();
        }
    while (proc >= layout_.shape())
        {
        proc -= layout_.shape();
        }
    return proc;
    }

int ParallelMesh::findProcessor(int idx) const
    {
    if (idx < 0 || idx >= full_mesh_->shape())
        {
        // ERROR: processor out of range
        }

    // TODO: replace with binary search since vectors are sorted
    int proc=-1;
    for (int i=0; proc < 0 && i < layout_.shape(); ++i)
        {
        if (idx >= starts_[layout_(i)] && idx < ends_[layout_(i)])
            {
            proc = i;
            }
        }

    return proc;
    }

void ParallelMesh::sync(std::shared_ptr<Field> field)
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
    if (comm_->size() > 1)
        {
        const int left = layout_(getProcessorCoordinatesByOffset(-1));
        const int right = layout_(getProcessorCoordinatesByOffset(1));

        MPI_Comm comm = comm_->get();
        MPI_Request requests[4];
        // receive left buffer from left (tag 0), right buffer from right (tag 1)
        MPI_Irecv(&f(-buffer_shape),buffer_shape,MPI_DOUBLE,left,0,comm,&requests[0]);
        MPI_Irecv(&f(shape),buffer_shape,MPI_DOUBLE,right,1,comm,&requests[1]);
        // send left edge to left (tag 1), right edge to right (tag 0)
        MPI_Isend(&f(0),buffer_shape,MPI_DOUBLE,left,1,comm,&requests[2]);
        MPI_Isend(&f(shape-buffer_shape),buffer_shape,MPI_DOUBLE,right,0,comm,&requests[3]);
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

void ParallelMesh::setup(std::shared_ptr<Mesh> mesh, std::shared_ptr<Communicator> comm)
    {
    // set the *full* mesh passed to the communicator
    full_mesh_ = mesh;

    // set the communicator
    comm_ = comm;
    layout_ = DataLayout(comm_->size());
    coords_ = comm_->rank();

    // determine the mesh sites that each processor owns
    const int floor_shape = full_mesh_->shape()/layout_.shape();
    const int leftover = full_mesh_->shape() - layout_.shape()*floor_shape;
    starts_ = std::vector<int>(layout_.size(),0);
    ends_ = std::vector<int>(layout_.size(),0);
    for (int idx=0; idx < layout_.shape(); ++idx)
        {
        const int coord_idx = layout_(idx);
        if (idx < leftover)
            {
            starts_[coord_idx] = idx*floor_shape+idx;
            ends_[coord_idx] = starts_[coord_idx] + (floor_shape+1);
            }
        else
            {
            starts_[coord_idx] = idx*floor_shape+leftover;
            ends_[coord_idx] = starts_[coord_idx] + floor_shape;
            }
        }

    // size the local mesh based on the sites covered
    const int start = starts_[layout_(coords_)];
    const int end = ends_[layout_(coords_)];
    if (start == end)
        {
        // ERROR: cannot have empty processor
        }
    local_mesh_ = std::make_shared<Mesh>(end-start,full_mesh_->step(),full_mesh_->asLength(start));
    }

}
