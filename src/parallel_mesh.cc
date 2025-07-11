#include "flyft/parallel_mesh.h"

#include <assert.h>
#include <stdexcept>

namespace flyft
    {

ParallelMesh::ParallelMesh(std::shared_ptr<Mesh> mesh, std::shared_ptr<Communicator> comm)
    : comm_(comm), full_mesh_(mesh)
    {
    // validate and set the *full* mesh passed to the communicator
    assert(full_mesh_->num_position_coordinates() == 1);
    if (!full_mesh_->validate_boundary_conditions())
        {
        throw std::runtime_error("Invalid boundary conditions for mesh");
        }

    // determine the mesh cells that each processor owns
    const auto num_procs = comm_->size();
    const int floor_shape = full_mesh_->shape()[0] / num_procs;
    const int leftover = full_mesh_->shape()[0] - num_procs * floor_shape;
    starts_ = std::vector<int>(num_procs, 0);
    ends_ = std::vector<int>(num_procs, 0);
    for (int idx = 0; idx < num_procs; ++idx)
        {
        if (idx < leftover)
            {
            starts_[idx] = idx * floor_shape + idx;
            ends_[idx] = starts_[idx] + (floor_shape + 1);
            }
        else
            {
            starts_[idx] = idx * floor_shape + leftover;
            ends_[idx] = starts_[idx] + floor_shape;
            }
        }

    // size the local mesh based on the sites covered
    const auto rank = comm_->rank();
    const int start = starts_[rank];
    const int end = ends_[rank];
    if (start == end)
        {
        throw std::runtime_error("Empty processor sized");
        }

    local_mesh_ = full_mesh_->slice(start, end);
    }

ParallelMesh::~ParallelMesh() {}

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

int ParallelMesh::num_position_coordinates() const
    {
    return full_mesh_->num_position_coordinates();
    }

int ParallelMesh::num_orientation_coordinates() const
    {
    return full_mesh_->num_orientation_coordinates();
    }

int ParallelMesh::num_coordinates() const
    {
    return full_mesh_->num_coordinates();
    }

int ParallelMesh::getProcessorShape() const
    {
    return comm_->size();
    }

int ParallelMesh::getProcessorCoordinates() const
    {
    return comm_->rank();
    }

int ParallelMesh::getProcessorCoordinatesByOffset(int offset) const
    {
    int proc = comm_->rank() + offset;
    const auto num_ranks = comm_->size();
    while (proc < 0)
        {
        proc += num_ranks;
        }
    while (proc >= num_ranks)
        {
        proc -= num_ranks;
        }
    assert(proc >= 0 && proc < num_ranks);
    return proc;
    }

int ParallelMesh::findProcessor(const int* cell) const
    {
    if (cell[0] < 0 || cell[0] >= full_mesh_->shape()[0])
        {
        throw std::runtime_error("Cell out of range");
        }

    // TODO: replace with binary search since vectors are sorted
    const auto num_ranks = comm_->size();
    int proc = -1;
    for (int i = 0; proc < 0 && i < num_ranks; ++i)
        {
        if (cell[0] >= starts_[i] && cell[0] < ends_[i])
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

    // check field shape in decomposed dimension
    const int shape = field->shape()[0];
    const int buffer_shape = field->buffer_shape()[0];
    if (buffer_shape > shape)
        {
        throw std::runtime_error(
            "Mesh overdecomposed, only nearest neighbor communication supported.");
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
    const int left = getProcessorCoordinatesByOffset(-1);
    const int right = getProcessorCoordinatesByOffset(1);
    std::vector<MPI_Request> requests;
    requests.reserve(4);

    // stride for sending data of decomposed coordinate
    size_t decomp_stride = 1;

    if (comm_->size() > 1
        && (lower_bc == BoundaryType::periodic || lower_bc == BoundaryType::internal))
        {
        // calculate stride per position coordinate
        const auto orientation_shape = full_mesh_->shape() + full_mesh_->num_position_coordinates();
        for (int dim = 0; dim < full_mesh_->num_orientation_coordinates(); ++dim)
            {
            decomp_stride *= orientation_shape[dim];
            }

        // get iterators (pointers) to portions of data we will send / receive from
        std::vector<int> multi_index(f.num_dimensions(), 0);
        multi_index[0] = -buffer_shape;
        const auto recv_it = f.make_iterator(multi_index.data());
        multi_index[0] = 0;
        const auto send_it = f.make_iterator(multi_index.data());

        // receive left buffer from left (tag 0), right buffer from right (tag 1)
        MPI_Comm comm = comm_->get();
        const auto end = requests.size();
        requests.resize(end + 2);
        MPI_Irecv(recv_it.get(),
                  buffer_shape * decomp_stride,
                  MPI_DOUBLE,
                  left,
                  0,
                  comm,
                  &requests[end]);
        MPI_Isend(send_it.get(),
                  buffer_shape * decomp_stride,
                  MPI_DOUBLE,
                  left,
                  1,
                  comm,
                  &requests[end + 1]);
        }
    else
#endif
        {
        std::vector<int> multi_index(f.num_dimensions(), 0);
        if (lower_bc == BoundaryType::zero)
            {
            multi_index[0] = -buffer_shape;
            auto recv_it_start = f.make_iterator(multi_index.data());

            multi_index[0] = 0;
            const auto recv_it_end = f.make_iterator(multi_index.data());

            while (recv_it_start != recv_it_end)
                {
                *recv_it_start = 0;
                ++recv_it_start;
                }
            }
        else if (lower_bc == BoundaryType::repeat || lower_bc == BoundaryType::reflect)
            {
            for (int idx = 0; idx < buffer_shape; ++idx)
                {
                multi_index[0] = -1 - idx;
                auto recv_it_start = f.make_iterator(multi_index.data());

                ++multi_index[0];
                const auto recv_it_end = f.make_iterator(multi_index.data());

                if (lower_bc == BoundaryType::repeat)
                    {
                    multi_index[0] = 0;
                    }
                else // reflect
                    {
                    multi_index[0] = 1 + idx;
                    }
                auto send_it_start = f.make_iterator(multi_index.data());

                while (recv_it_start != recv_it_end)
                    {
                    *recv_it_start = *send_it_start;
                    ++recv_it_start;
                    ++send_it_start;
                    }
                }
            }
        else // periodic / internal
            {
            multi_index[0] = -buffer_shape;
            auto recv_it_start = f.make_iterator(multi_index.data());

            multi_index[0] = shape - buffer_shape;
            auto send_it_start = f.make_iterator(multi_index.data());

            multi_index[0] = shape;
            const auto send_it_end = f.make_iterator(multi_index.data());

            while (send_it_start != send_it_end)
                {
                *recv_it_start = *send_it_start;
                ++recv_it_start;
                ++send_it_start;
                }
            }
        }

#ifdef FLYFT_MPI
    if (comm_->size() > 1
        && (upper_bc == BoundaryType::periodic || upper_bc == BoundaryType::internal))
        {
        // get iterators (pointers) to portions of data we will send / receive from
        std::vector<int> multi_index(f.num_dimensions(), 0);
        multi_index[0] = shape;
        const auto recv_it = f.make_iterator(multi_index.data());
        multi_index[0] = shape - buffer_shape;
        const auto send_it = f.make_iterator(multi_index.data());

        // send left edge to left (tag 1), right edge to right (tag 0)
        MPI_Comm comm = comm_->get();
        const auto end = requests.size();
        requests.resize(end + 2);
        MPI_Irecv(recv_it.get(),
                  buffer_shape * decomp_stride,
                  MPI_DOUBLE,
                  right,
                  1,
                  comm,
                  &requests[end]);
        MPI_Isend(send_it.get(),
                  buffer_shape * decomp_stride,
                  MPI_DOUBLE,
                  right,
                  0,
                  comm,
                  &requests[end + 1]);
        }
    else
#endif
        {
        std::vector<int> multi_index(f.num_dimensions(), 0);
        if (lower_bc == BoundaryType::zero)
            {
            multi_index[0] = shape;
            auto recv_it_start = f.make_iterator(multi_index.data());

            multi_index[0] = shape + buffer_shape;
            const auto recv_it_end = f.make_iterator(multi_index.data());

            while (recv_it_start != recv_it_end)
                {
                *recv_it_start = 0;
                ++recv_it_start;
                }
            }
        else if (lower_bc == BoundaryType::repeat || lower_bc == BoundaryType::reflect)
            {
            for (int idx = 0; idx < buffer_shape; ++idx)
                {
                multi_index[0] = shape + idx;
                auto recv_it_start = f.make_iterator(multi_index.data());

                ++multi_index[0];
                const auto recv_it_end = f.make_iterator(multi_index.data());

                if (lower_bc == BoundaryType::repeat)
                    {
                    multi_index[0] = shape - 1;
                    }
                else // reflect
                    {
                    multi_index[0] = shape - 1 - idx;
                    }
                auto send_it_start = f.make_iterator(multi_index.data());

                while (recv_it_start != recv_it_end)
                    {
                    *recv_it_start = *send_it_start;
                    ++recv_it_start;
                    ++send_it_start;
                    }
                }
            }
        else // periodic
            {
            multi_index[0] = shape;
            auto recv_it_start = f.make_iterator(multi_index.data());

            multi_index[0] = -buffer_shape;
            auto send_it_start = f.make_iterator(multi_index.data());

            multi_index[0] = 0;
            const auto send_it_end = f.make_iterator(multi_index.data());

            while (send_it_start != send_it_end)
                {
                *recv_it_start = *send_it_start;
                ++send_it_start;
                ++recv_it_start;
                }
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
        MPI_Waitall(requests.size(), &requests[0], MPI_STATUSES_IGNORE);
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
        MPI_Waitall(requests.size(), &requests[0], MPI_STATUSES_IGNORE);
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
        // calculate stride per position coordinate
        size_t decomp_stride = 1;
        const auto orientation_shape = full_mesh_->shape() + full_mesh_->num_position_coordinates();
        for (int dim = 0; dim < full_mesh_->num_orientation_coordinates(); ++dim)
            {
            decomp_stride *= orientation_shape[dim];
            }

        // determine number of elements sent by each rank
        const auto num_procs = comm_->size();
        std::vector<int> counts;
        std::vector<int> displs;

        // send buffer is valid on all ranks
        const auto f = field->const_view();

        // new field will only be alloc'd on the root rank and invalid on others
        // receive buffer is only valid on the root rank
        void* recv(nullptr);
        if (comm_->rank() == root)
            {
            new_field = std::make_shared<Field>(full_mesh_->num_coordinates(), full_mesh_->shape());

            recv = static_cast<void*>(new_field->view().begin().get());
            counts.resize(num_procs);
            displs.resize(num_procs);
            for (int idx = 0; idx < num_procs; ++idx)
                {
                counts[idx] = (ends_[idx] - starts_[idx]) * decomp_stride;
                if (idx > 0)
                    {
                    displs[idx] = displs[idx - 1] + counts[idx];
                    }
                }
            }

        // gather to the root rank
        MPI_Gatherv(f.begin().get(),
                    f.size(),
                    MPI_DOUBLE,
                    recv,
                    counts.data(),
                    displs.data(),
                    MPI_DOUBLE,
                    root,
                    comm_->get());
        }
    else
#endif
        {
        new_field = field;
        }

    return new_field;
    }

    } // namespace flyft
