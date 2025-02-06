#include "flyft/functional.h"

#include <algorithm>

namespace flyft
    {

Functional::Functional() : value_(std::nan("")) {}

Functional::~Functional() {}

Functional::Token Functional::compute(std::shared_ptr<State> state, bool compute_value)
    {
    bool needs_compute = setup(state, compute_value);
    if (!needs_compute)
        {
        return token_;
        }
    else
        {
        // overlap any synchronization of the fields that may be requested if this functional
        // does not need a field buffer itself, as it should be nearly "free"
        bool force_sync = !needsBuffer(state);
        if (force_sync)
            {
            state->startSyncFields();
            }

        _compute(state, compute_value);

        if (force_sync)
            {
            state->endSyncFields();
            }
        return finalize(state, compute_value);
        }
    }

double Functional::getValue() const
    {
    return value_;
    }

const TypeMap<std::shared_ptr<Field>>& Functional::getDerivatives()
    {
    return derivatives_;
    }

std::shared_ptr<Field> Functional::getDerivative(const std::string& type)
    {
    return derivatives_(type);
    }

std::shared_ptr<const Field> Functional::getDerivative(const std::string& type) const
    {
    return derivatives_(type);
    }

void Functional::requestDerivativeBuffer(const std::string& type, int buffer_request)
    {
    auto it = buffer_requests_.find(type);
    if (it == buffer_requests_.end() || buffer_request > it->second)
        {
        buffer_requests_[type] = buffer_request;
        }
    }

bool Functional::needsBuffer(std::shared_ptr<State> state)
    {
    bool needs_buffer = false;
    for (const auto& t : state->getTypes())
        {
        int field_buffer = determineBufferShape(state, t);
        if (field_buffer != 0)
            {
            needs_buffer = true;
            break;
            }
        }
    return needs_buffer;
    }

int Functional::determineBufferShape(std::shared_ptr<State> /*state*/, const std::string& /*type*/)
    {
    return 0;
    }

bool Functional::setup(std::shared_ptr<State> state, bool compute_value)
    {
    // sync required fields
    for (const auto& t : state->getTypes())
        {
        state->requestFieldBuffer(t, determineBufferShape(state, t));
        }

    // match derivatives to state types, and attach as dependencies
    TypeMap<Identifier> deriv_ids;
    for (const auto& it : derivatives_)
        {
        deriv_ids[it.first] = it.second->id();
        }
    state->matchFields(derivatives_, buffer_requests_);
    for (const auto& it : deriv_ids)
        {
        // type removed or field is a new object
        if (!derivatives_.contains(it.first) || derivatives_[it.first]->id() != it.second)
            {
            compute_depends_.remove(it.second);
            }
        }
    for (const auto& it : derivatives_)
        {
        // attach the active objects (will do nothing if object is already a dependency)
        compute_depends_.add(it.second.get());
        }

    // return whether evaluation is required
    bool compute = ((!compute_token_ || token_ != compute_token_)
                    || (!compute_state_token_ || state->token() != compute_state_token_)
                    || compute_depends_.changed() || (compute_value && std::isnan(value_)));

    return compute;
    }

Functional::Token Functional::finalize(std::shared_ptr<State> state, bool compute_value)
    {
    // make sure value has NaN if not computed
    if (!compute_value)
        {
        value_ = std::nan("");
        }

    // stage changes after a compute
    token_.stageAndCommit();

    // capture dependencies
    compute_token_ = token_;
    compute_state_token_ = state->token();
    compute_depends_.capture();

    return compute_token_;
    }

    } // namespace flyft
