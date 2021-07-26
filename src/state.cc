#include "flyft/state.h"

#include <algorithm>
#include <iostream>

namespace flyft
{

State::State(std::shared_ptr<Mesh> mesh, const std::string& type)
    : State(mesh, std::vector<std::string>({type}))
    {}

State::State(std::shared_ptr<Mesh> mesh, const std::vector<std::string>& types)
    : mesh_(mesh), types_(types), time_(0)
    {
    for (const auto& t : types_)
        {
        fields_[t] = std::make_shared<Field>(mesh_->layout());
        }
    }

State::State(const State& other)
    : mesh_(other.mesh_),
      types_(other.types_),
      time_(other.time_)
    {
    for (const auto& t : types_)
        {
        fields_[t] = std::make_shared<Field>(mesh_->layout());
        std::copy(other.fields_.at(t)->cbegin_full(),other.fields_.at(t)->cend_full(),fields_.at(t)->begin_full());
        }
    }

State::State(State&& other)
    : mesh_(std::move(other.mesh_)),
      types_(std::move(other.types_)),
      fields_(std::move(other.fields_)),
      time_(std::move(other.time_))
    {
    }

State& State::operator=(const State& other)
    {
    if (&other != this)
        {
        mesh_ = other.mesh_;
        types_ = other.types_;
        syncFields(fields_);
        for (const auto& t : types_)
            {
            std::copy(other.fields_.at(t)->cbegin_full(), other.fields_.at(t)->cend_full(), fields_.at(t)->begin_full());
            }
        time_ = other.time_;
        }
    return *this;
    }

State& State::operator=(State&& other)
    {
    mesh_ = std::move(other.mesh_);
    types_ = std::move(other.types_);
    fields_ = std::move(other.fields_);
    time_ = std::move(other.time_);
    return *this;
    }

std::shared_ptr<Mesh> State::getMesh()
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
            fields[t] = std::make_shared<Field>(mesh_->layout());
            }
        else
            {
            fields[t]->reshape(mesh_->layout());
            }
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

void State::requestBuffer(double buffer_request)
    {
    if (buffer_request > mesh_->buffer())
        {
        mesh_->setBuffer(buffer_request);
        }

    const auto layout = mesh_->layout();

    // TODO: remove redundant calls here, just in place for testing
    // force update to field layouts
    syncFields(fields_);

    // copy buffers
    for (const auto& t : types_)
        {
        auto f = fields_[t];
        // copy from left side to right
        std::copy(f->cbegin(),
                  f->cbegin()+layout.buffer_shape(),
                  f->end());
        // copy from right side to left
        std::copy(f->cend()-layout.buffer_shape(),
                  f->cend(),
                  f->begin()-layout.buffer_shape());
        }
    }

}
