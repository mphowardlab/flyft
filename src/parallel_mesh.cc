#include "flyft/parallel_mesh.h"

namespace flyft
{

ParallelMesh::ParallelMesh(std::shared_ptr<const Mesh> mesh)
    {
    // initialize the MPI communicator (Cartesian ordering is row-major like DataLayout)
    layout_ = DataLayout(1);
    rank_ = 0;
    coords_ = 0;

    // setup the mesh decomposition
    setMesh(mesh);
    }

ParallelMesh::~ParallelMesh()
    {
    }

void ParallelMesh::sync(std::shared_ptr<Field> field)
    {
    // check if field was recently synced and stop if not needed
    auto token = field_tokens_.find(field->id());
    if (token != field_tokens_.end() && token->second == field->token())
        {
        return;
        }

    // sync field
    const auto f = field->view();
    const int shape = field->shape();
    const int buffer_shape = field->buffer_shape();

    if (buffer_shape > local_mesh_->shape())
        {
        // ERROR: overdecomposed (only nearest-neighbor comms supported)
        }

    for (int idx=0; idx < buffer_shape; ++idx)
        {
        f(shape+idx) = f(idx);
        f(-1-idx) = f(shape-1-idx);
        }

    // cache token
    field_tokens_[field->id()] = field->token();
    }

int ParallelMesh::getNumProcessors() const
    {
    return layout_.size();
    }

int ParallelMesh::getProcessorShape() const
    {
    return layout_.shape();
    }

int ParallelMesh::getProcessor() const
    {
    return rank_;
    }

int ParallelMesh::getProcessorCoordinates() const
    {
    return coords_;
    }

int ParallelMesh::findProcessor(int idx) const
    {
    if (idx < 0 || idx >= global_mesh_->shape())
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

std::shared_ptr<const Mesh> ParallelMesh::local() const
    {
    return local_mesh_;
    }

std::shared_ptr<const Mesh> ParallelMesh::global() const
    {
    return global_mesh_;
    }

void ParallelMesh::setMesh(std::shared_ptr<const Mesh> mesh)
    {
    // set the *global* mesh passed to the communicator
    global_mesh_ = mesh;

    // determine the mesh sites that each processor owns
    const int floor_shape = global_mesh_->shape()/layout_.shape();
    const int leftover = global_mesh_->shape() - layout_.shape()*floor_shape;
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
    local_mesh_ = std::make_shared<const Mesh>(end-start,global_mesh_->step(),global_mesh_->asLength(start));
    }

}
