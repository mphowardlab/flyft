#include "flyft/state.h"

namespace flyft
{

State::State(std::shared_ptr<Mesh> mesh, int num_fields)
    : mesh_(mesh), num_fields_(num_fields), fields_(num_fields)
    {
    for (auto& f : fields_)
        {
        f = std::make_shared<Field>(mesh_);
        }
    }

std::shared_ptr<Mesh> State::getMesh()
    {
    return mesh_;
    }

int State::getNumFields() const
    {
    return static_cast<int>(fields_.size());
    }

const std::vector<std::shared_ptr<Field>>& State::getFields()
    {
    return fields_;
    }

std::shared_ptr<Field> State::getField(int idx)
    {
    return fields_[idx];
    }

std::shared_ptr<const Field> State::getField(int idx) const
    {
    return std::static_pointer_cast<const Field>(fields_[idx]);
    }


void State::setField(int idx, std::shared_ptr<Field> field)
    {
    fields_[idx] = field;
    }

}
