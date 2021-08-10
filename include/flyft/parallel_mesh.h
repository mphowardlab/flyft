#ifndef FLYFT_PARALLEL_MESH_H_
#define FLYFT_PARALLEL_MESH_H_

#include "flyft/communicator.h"
#include "flyft/data_token.h"
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
        ParallelMesh(std::shared_ptr<const Mesh> mesh, std::shared_ptr<const Communicator> comm);
        virtual ~ParallelMesh();

        std::shared_ptr<const Communicator> getCommunicator() const;

        std::shared_ptr<const Mesh> local() const;
        std::shared_ptr<const Mesh> full() const;

        void sync(std::shared_ptr<Field> field);

        int getProcessorShape() const;
        int getProcessorCoordinates() const;
        int findProcessor(int idx) const;

    private:
        std::shared_ptr<const Communicator> comm_;
        DataLayout layout_;
        int coords_;

        std::shared_ptr<const Mesh> full_mesh_;
        std::shared_ptr<const Mesh> local_mesh_;
        std::vector<int> starts_;
        std::vector<int> ends_;

        std::unordered_map<Field::Identifier,DataToken> field_tokens_;

        void setup(std::shared_ptr<const Mesh> mesh, std::shared_ptr<const Communicator> comm);
    };

}

#endif // FLYFT_PARALLEL_MESH_H_