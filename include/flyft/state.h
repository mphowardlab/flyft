#ifndef FLYFT_STATE_H_
#define FLYFT_STATE_H_

#include "flyft/field.h"
#include "flyft/mesh.h"

#include <memory>
#include <vector>

namespace flyft
{

class State
    {
    public:
        State() = delete;
        State(std::shared_ptr<Mesh> mesh, int num_fields);

        // noncopyable / nonmovable
        State(const State&) = delete;
        State(State&&) = delete;
        State& operator=(const State&) = delete;
        State& operator=(State&&) = delete;

        std::shared_ptr<Mesh> getMesh();

        int getNumFields() const;
        const std::vector<std::shared_ptr<Field>>& getFields();
        std::shared_ptr<Field> getField(int idx);
        std::shared_ptr<const Field> getField(int idx) const;
        void setField(int idx, std::shared_ptr<Field> field);

    private:
        std::shared_ptr<Mesh> mesh_;

        int num_fields_;
        std::vector<std::shared_ptr<Field>> fields_;
    };

}

#endif // FLYFT_STATE_H_
