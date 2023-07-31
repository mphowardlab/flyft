#include "flyft/parallel_mesh.h"

#include <stdexcept>

namespace flyft
{

ParallelMesh::ParallelMesh(std::shared_ptr<Mesh> mesh, std::shared_ptr<Communicator> comm)
    {
    // set the *full* mesh passed to the communicator
    full_mesh_ = mesh;

    // set the communicator (coordinates are assumed to be the same as rank for now)
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

    local_mesh_ = full_mesh_->slice(start, end);
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
    startSync(field);
    endSync(field);
    }

void ParallelMesh::startSync(std::shared_ptr<Field> field)
    {
    // check if field was recently synced and stop if not needed
    auto token = field_tokens_.find(field->id());
    if (token != field_tokens_.end() && token->second == field->token())
        {
        return;
        }

    // make sure field is not currently in flight before we do anything
    #ifdef FLYFT_MPI
    if (field_requests_.find(field->id()) != field_requests_.end())
        {
        throw std::runtime_error("Cannot sync field, data already in flight.");
        }
    #endif

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
    auto f = field->view();
    const auto lower_bc = local_mesh_->lower_boundary_condition();
    const auto upper_bc = local_mesh_->upper_boundary_condition();

    #ifdef FLYFT_MPI
    const int left = layout_(getProcessorCoordinatesByOffset(-1));
    const int right = layout_(getProcessorCoordinatesByOffset(1));
    std::vector<MPI_Request> requests;
    requests.reserve(4);

    if (comm_->size() > 1 && (lower_bc==BoundaryType::periodic || lower_bc==BoundaryType::internal))
        {
        // receive left buffer from left (tag 0), right buffer from right (tag 1)
        MPI_Comm comm = comm_->get();
        const auto end = requests.size();
        requests.resize(end+2);
        MPI_Irecv(&f(-buffer_shape),buffer_shape,MPI_DOUBLE,left,0,comm,&requests[end]);
        MPI_Isend(&f(0),buffer_shape,MPI_DOUBLE,left,1,comm,&requests[end+1]);
        }
    else
    #endif
        {
        for(int idx = 0; idx < buffer_shape; ++idx)
            {
            double value;
            if (lower_bc == BoundaryType::zero)
                {
                value = 0;
                }
            else if (lower_bc == BoundaryType::repeat)  
                {
                value = f(0);
                }    
            else if (lower_bc == BoundaryType::reflect)
                {
                value = f(1+idx);
                }
            else if (lower_bc == BoundaryType::periodic || lower_bc == BoundaryType::internal)
                {
                value = f(shape-1-idx);
                }
            else
                {
                throw std::runtime_error("Unknown boundary condition");
                }
            f(-1-idx) = value;
            }
        }

    #ifdef FLYFT_MPI
    if(comm_->size() > 1 && (upper_bc == BoundaryType::periodic || upper_bc==BoundaryType::internal))
        {
        // send left edge to left (tag 1), right edge to right (tag 0)
        MPI_Comm comm = comm_->get();
        const auto end = requests.size();
        requests.resize(end+2);
        MPI_Irecv(&f(shape),buffer_shape,MPI_DOUBLE,right,1,comm,&requests[end]);
        MPI_Isend(&f(shape-buffer_shape),buffer_shape,MPI_DOUBLE,right,0,comm,&requests[end+1]);
        }
    else
    #endif
        {
        for(int idx = 0; idx < buffer_shape; ++idx)
            {
            double value;
            if(upper_bc == BoundaryType::zero)
                {
                value = 0;
                }
            else if(upper_bc == BoundaryType::repeat)
                {
                value = f(shape);
                }
            else if(upper_bc == BoundaryType::reflect)
                {
                value = f(shape-1-idx);
                }
            else if(upper_bc == BoundaryType::periodic || upper_bc == BoundaryType::internal) 
                {
                value = f(idx);
                }
            else
                {
                throw std::runtime_error("Unknown boundary condition");
                }
            f(shape+idx) = value;
            }
        }

    #ifdef FLYFT_MPI
    if (requests.size() > 0)
        {
        field_requests_[field->id()] = requests;
        }
    #endif    
    // cache token
    field_tokens_[field->id()] = field->token();
    }
    

#ifdef FLYFT_MPI
void ParallelMesh::endSync(std::shared_ptr<Field> field)
#else
void ParallelMesh::endSync(std::shared_ptr<Field> /*field*/)
#endif
    {
    #ifdef FLYFT_MPI
    // wait for communication to finish
    auto it = field_requests_.find(field->id());
    if (it != field_requests_.end())
        {
        auto requests = it->second;
        MPI_Waitall(requests.size(),&requests[0],MPI_STATUSES_IGNORE);
        field_requests_.erase(it);
        }
    #endif // FLYFT_MPI
    }

void ParallelMesh::endSyncAll()
    {
    #ifdef FLYFT_MPI
    for (auto it = field_requests_.begin(); it != field_requests_.end(); /* no increment here */)
        {
        auto requests = it->second;
        MPI_Waitall(requests.size(),&requests[0],MPI_STATUSES_IGNORE);
        field_requests_.erase(it++);
        }
    #endif // FLYFT_MPI
    }

#ifdef FLYFT_MPI
std::shared_ptr<Field> ParallelMesh::gather(std::shared_ptr<Field> field, int root) const
#else
std::shared_ptr<Field> ParallelMesh::gather(std::shared_ptr<Field> field, int /*root*/) const
#endif
    {
    std::shared_ptr<Field> new_field;

    #ifdef FLYFT_MPI
    if (comm_->size() > 1)
        {
        // determine number of elements sent by each rank
        std::vector<int> counts(comm_->size());
        for (int idx=0; idx < layout_.shape(); ++idx)
            {
            const auto coord_idx = layout_(idx);
            counts[coord_idx] = ends_[coord_idx]-starts_[coord_idx];
            }

        // send buffer is valid on all ranks
        const auto f = field->const_view();

        // new field will only be alloc'd on the root rank and invalid on others
        // receive buffer is only valid on the root rank
        void* recv(nullptr);
        if (comm_->rank() == root)
            {
            new_field = std::make_shared<Field>(full_mesh_->shape());
            auto new_f = new_field->view();
            recv = static_cast<void*>(&new_f(0));
            }

        // gather to the root rank
        MPI_Gatherv(&f(0),f.size(),MPI_DOUBLE,recv,&counts[0],&starts_[0],MPI_DOUBLE,root,comm_->get());
        }
    else
    #endif
        {
        new_field = field;
        }

    return new_field;
    }

}
