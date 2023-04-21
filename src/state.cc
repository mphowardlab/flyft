#include "flyft/state.h"

#include <algorithm>
#include <cmath>

namespace flyft
{

State::State(std::shared_ptr<ParallelMesh> mesh, const std::string& type)
    : State(mesh, std::vector<std::string>({type}))
    {}

State::State(std::shared_ptr<ParallelMesh> mesh, const std::vector<std::string>& types)
    : mesh_(mesh),types_(types),time_(0)
    {
    for (const auto& t : types_)
        {
        fields_[t] = std::make_shared<Field>(mesh_->local()->shape());
        depends_.add(fields_[t].get());
        }
    }

State::State(const State& other)
    : TrackedObject(other),
      mesh_(other.mesh_),
      types_(other.types_),
      time_(other.time_)
    {
    for (const auto& t : types_)
        {
        auto other_field = other.fields_(t);
        fields_[t] = std::make_shared<Field>(other_field->shape(),other_field->buffer_shape());
        std::copy(other_field->const_full_view().begin(),other_field->const_full_view().end(),fields_[t]->full_view().begin());
        depends_.add(fields_[t].get());
        }
    }

State::State(State&& other)
    : TrackedObject(other),
      mesh_(std::move(other.mesh_)),
      types_(std::move(other.types_)),
      fields_(std::move(other.fields_)),
      time_(std::move(other.time_))
    {
    }

State& State::operator=(const State& other)
    {
    if (this != &other)
        {
        TrackedObject::operator=(other);
        mesh_ = other.mesh_;
        types_ = other.types_;
        time_ = other.time_;

        // match field types and buffer shapes
        TypeMap<int> buffers;
        for (const auto& t : types_)
            {
            buffers[t] = other.fields_(t)->buffer_shape();
            }
        other.matchFields(fields_,buffers);

        // copy contents of other fields
        depends_.clear();
        for (const auto& t : types_)
            {
            auto other_field = other.fields_(t);
            std::copy(other_field->const_full_view().begin(),other_field->const_full_view().end(),fields_[t]->full_view().begin());
            depends_.add(fields_[t].get());
            }
        }
    return *this;
    }

State& State::operator=(State&& other)
    {
    TrackedObject::operator=(other);
    mesh_ = std::move(other.mesh_);
    types_ = std::move(other.types_);
    fields_ = std::move(other.fields_);
    time_ = std::move(other.time_);
    return *this;
    }

std::shared_ptr<ParallelMesh> State::getMesh()
    {
    return mesh_;
    }

std::shared_ptr<const ParallelMesh> State::getMesh() const
    {
    return mesh_;
    }

std::shared_ptr<Communicator> State::getCommunicator()
    {
    return mesh_->getCommunicator();
    }

std::shared_ptr<const Communicator> State::getCommunicator() const
    {
    return mesh_->getCommunicator();
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
    return fields_(type);
    }

void State::requestFieldBuffer(const std::string& type, int buffer_request)
    {
    fields_(type)->requestBuffer(buffer_request);
    }

TypeMap<std::shared_ptr<Field>> State::gatherFields(int rank) const
    {
    TypeMap<std::shared_ptr<Field>> fields;
    for (const auto& t : types_)
        {
        fields[t] = gatherField(t,rank);
        }
    return fields;
    }

std::shared_ptr<Field> State::gatherField(const std::string& type, int rank) const
    {
    return mesh_->gather(fields_(type),rank);
    }

void State::syncFields()
    {
    syncFields(fields_);
    }

void State::startSyncFields()
    {
    startSyncFields(fields_);
    }

void State::endSyncFields()
    {
    endSyncFields(fields_);
    }

void State::syncFields(const TypeMap<std::shared_ptr<Field>>& fields) const
    {
    startSyncFields(fields);
    endSyncFields(fields);
    }

void State::startSyncFields(const TypeMap<std::shared_ptr<Field>>& fields) const
    {
    for (auto it=fields.cbegin(); it != fields.cend(); ++it)
        {
        if (std::find(types_.begin(), types_.end(), it->first) == types_.end())
            {
            // ERROR: types do not match
            }
        else
            {
            mesh_->startSync(it->second);
            }
        }
    }

void State::endSyncFields(const TypeMap<std::shared_ptr<Field>>& fields) const
    {
    for (auto it=fields.cbegin(); it != fields.cend(); ++it)
        {
        if (std::find(types_.begin(), types_.end(), it->first) == types_.end())
            {
            // ERROR: types do not match
            }
        else
            {
            mesh_->endSync(it->second);
            }
        }
    }

void State::endSyncAll() const
    {
    mesh_->endSyncAll();
    }

void State::matchFields(TypeMap<std::shared_ptr<Field>>& fields) const
    {
    matchFields(fields,TypeMap<int>());
    }

void State::matchFields(TypeMap<std::shared_ptr<Field>>& fields, const TypeMap<int>& buffer_requests) const
    {
    // purge stored types that are not in the state
    for (auto it=fields.cbegin(); it != fields.cend(); /* no increment here */)
        {
        const auto t = it->first;
        if (std::find(types_.begin(), types_.end(), t) == types_.end())
            {
            it = fields.erase(it);
            }
        else
            {
            ++it;
            }
        }

    // ensure every type has a field with the right shape and buffer
    for (const auto& t : types_)
        {
        // check if field exists already
        bool has_type = (fields.find(t) != fields.end());

        // determine buffer request, preserving buffer if type already exists but no request is made
        int buffer_request;
        auto it = buffer_requests.find(t);
        if (it != buffer_requests.cend())
            {
            buffer_request = it->second;
            }
        else if (has_type)
            {
            buffer_request = fields[t]->buffer_shape();
            }
        else
            {
            buffer_request = 0;
            }

        // make sure field exists and has the right shape
        if (!has_type)
            {
            fields[t] = std::make_shared<Field>(mesh_->local()->shape(), buffer_request);
            }
        else
            {
            fields[t]->reshape(mesh_->local()->shape(), buffer_request);
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
    token_.stageAndCommit();
    }

void State::advanceTime(double timestep)
    {
    setTime(time_+timestep);
    }

State::Token State::token()
    {
    if (depends_.changed())
        {
        depends_.capture();
        token_.stageAndCommit();
        }
    return token_;
    }

}
