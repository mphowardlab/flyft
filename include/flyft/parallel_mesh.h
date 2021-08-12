#ifndef FLYFT_PARALLEL_MESH_H_
#define FLYFT_PARALLEL_MESH_H_

#include "flyft/communicator.h"
#include "flyft/data_token.h"
#include "flyft/field.h"
#include "flyft/mesh.h"

#include <memory>
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
        int findProcessor(int idx) const;

    private:
        std::shared_ptr<Communicator> comm_;
        DataLayout layout_;
        int coords_;

        std::shared_ptr<Mesh> full_mesh_;
        std::shared_ptr<Mesh> local_mesh_;
        std::vector<int> starts_;
        std::vector<int> ends_;

        void setup(std::shared_ptr<Mesh> mesh, std::shared_ptr<Communicator> comm);
    };

}

#endif // FLYFT_PARALLEL_MESH_H_
