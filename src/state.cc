#include "flyft/state.h"

namespace flyft
{

State::State(std::shared_ptr<const Mesh> mesh, int num_fields)
    : mesh_(mesh), num_fields_(num_fields), fields_(num_fields)
    {
    for (auto& f : fields_)
        {
        f = std::make_shared<Field>(mesh_->shape());
        }
    }

std::shared_ptr<const Mesh> State::getMesh() const
    {
    return mesh_;
    }

int State::getNumFields() const
    {
    return num_fields_;
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
    return fields_[idx];
    }


void State::setField(int idx, std::shared_ptr<Field> field)
    {
    fields_[idx] = field;
    }

}
