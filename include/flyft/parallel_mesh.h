#ifndef FLYFT_PARALLEL_MESH_H_
#define FLYFT_PARALLEL_MESH_H_

#include "flyft/communicator.h"
#include "flyft/field.h"
#include "flyft/mesh.h"

#include <memory>
#include <unordered_map>
#include <vector>

namespace flyft
    {

class ParallelMesh
    {
    public:
    ParallelMesh() = delete;
    ParallelMesh(std::shared_ptr<Mesh> mesh, std::shared_ptr<Communicator> comm);
    virtual ~ParallelMesh();

    std::shared_ptr<Communicator> getCommunicator();
    std::shared_ptr<const Communicator> getCommunicator() const;

    std::shared_ptr<const Mesh> local() const;
    std::shared_ptr<const Mesh> full() const;

    int getProcessorShape() const;
    int getProcessorCoordinates() const;
    int getProcessorCoordinatesByOffset(int offset) const;
    int findProcessor(const int* cell) const;

    void sync(std::shared_ptr<Field> field);
    void startSync(std::shared_ptr<Field> field);
    void endSync(std::shared_ptr<Field> field);
    void endSyncAll();

    std::shared_ptr<Field> gather(std::shared_ptr<Field> field, int root) const;

    private:
    std::shared_ptr<Communicator> comm_; //!< Communicator

    std::shared_ptr<Mesh> full_mesh_;
    std::shared_ptr<Mesh> local_mesh_;

    std::vector<int> starts_;
    std::vector<int> ends_;

    std::unordered_map<Field::Identifier, Field::Token> field_tokens_;
#ifdef FLYFT_MPI
    std::unordered_map<Field::Identifier, std::vector<MPI_Request>> field_requests_;
#endif // FLYFT_MPI
    };

    } // namespace flyft

#endif // FLYFT_PARALLEL_MESH_H_
