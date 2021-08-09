#ifndef FLYFT_PARALLEL_MESH_H_
#define FLYFT_PARALLEL_MESH_H_

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
        ParallelMesh(std::shared_ptr<const Mesh> mesh);
        virtual ~ParallelMesh();

        void sync(std::shared_ptr<Field> field);

        int getNumProcessors() const;
        int getProcessorShape() const;
        int getProcessor() const;
        int getProcessorCoordinates() const;

        int findProcessor(int idx) const;

        std::shared_ptr<const Mesh> local() const;
        std::shared_ptr<const Mesh> global() const;

    private:
        DataLayout layout_;
        int rank_;
        int coords_;

        std::shared_ptr<const Mesh> global_mesh_;
        std::shared_ptr<const Mesh> local_mesh_;
        std::vector<int> starts_;
        std::vector<int> ends_;

        std::unordered_map<Field::Identifier,DataToken> field_tokens_;

        void setMesh(std::shared_ptr<const Mesh> mesh);
    };

}

#endif // FLYFT_PARALLEL_MESH_H_
