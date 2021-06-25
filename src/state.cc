#include "flyft/state.h"

#include <algorithm>

namespace flyft
{

State::State(std::shared_ptr<const Mesh> mesh, const std::string& type)
    : State(mesh, std::vector<std::string>({type}))
    {}

State::State(std::shared_ptr<const Mesh> mesh, const std::vector<std::string>& types)
    : mesh_(mesh), types_(types), time_(0)
    {
    for (const auto& t : types_)
        {
        fields_[t] = std::make_shared<Field>(mesh_->shape());
        }
    }

std::shared_ptr<const Mesh> State::getMesh() const
    {
    return mesh_;
    }

int State::getNumFields() const
    {
    return static_cast<int>(types_.size());
    }

const std::vector<std::string>& State::getTypes()
    {
    return types_;
    }

const std::string State::getType(int idx) const
    {
    return types_.at(idx);
    }

int State::getTypeIndex(const std::string& type) const
    {
    const auto it = std::find(types_.begin(), types_.end(), type);
    if (it == types_.end())
        {
        // error: type not found
        }
    return (it - types_.begin());
    }

const TypeMap<std::shared_ptr<Field>>& State::getFields()
    {
    return fields_;
    }

std::shared_ptr<Field> State::getField(const std::string& type)
    {
    return fields_[type];
    }

std::shared_ptr<const Field> State::getField(const std::string& type) const
    {
    return fields_.at(type);
    }

void State::syncFields(TypeMap<std::shared_ptr<Field>>& fields) const
    {
    // purge stored types that are not in the state
    for (auto it = fields.cbegin(); it != fields.cend(); /* no increment here */)
        {
        auto t = it->first;
        if (std::find(types_.begin(), types_.end(), t) == types_.end())
            {
            it = fields.erase(it);
            }
        else
            {
            ++it;
            }
        }

    // ensure every type has a field with the right shape
    for (const auto& t : types_)
        {
        if (fields.find(t) == fields.end())
            {
            fields[t] = std::make_shared<Field>(mesh_->shape());
            }
        fields[t]->reshape(mesh_->shape());
        }
    }

double State::getTime() const
    {
    return time_;
    }

void State::setTime(double time)
    {
    time_ = time;
    }

void State::advanceTime(double timestep)
    {
    time_ += timestep;
    }

}
