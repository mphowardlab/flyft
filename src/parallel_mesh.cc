#include "flyft/parallel_mesh.h"

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
