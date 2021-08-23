#include "flyft/tracked_object.h"

#include <stdexcept>
#include <utility>

namespace flyft
{
TrackedObject::Identifier TrackedObject::count = 0;

TrackedObject::TrackedObject()
    : id_(count++), token_(id_)
    {}

TrackedObject::TrackedObject(const TrackedObject& /*other*/)
    : id_(count++), token_(id_)
    {}

TrackedObject::TrackedObject(TrackedObject&& other)
    : id_(std::move(other.id_)), token_(std::move(other.token_))
    {}

TrackedObject& TrackedObject::operator=(const TrackedObject& other)
    {
    if (this != &other)
        {
        token_.stage();
        }
    return *this;
    }

TrackedObject& TrackedObject::operator=(TrackedObject&& /*other*/)
    {
    token_.stage();
    return *this;
    }

TrackedObject::~TrackedObject()
    {}

TrackedObject::Identifier TrackedObject::id() const
    {
    return id_;
    }

const TrackedObject::Token& TrackedObject::token()
    {
    token_.commit();
    return token_;
    }

TrackedObject::Token::Token()
    : Token(invalid_id)
    {}

TrackedObject::Token::Token(TrackedObject::Identifier id)
    : Token(id,0)
    {}

TrackedObject::Token::Token(TrackedObject::Identifier id, uint64_t code)
    : id_(id), code_(code), dirty_(false)
    {}

TrackedObject::Token::~Token()
    {}

void TrackedObject::Token::commit()
    {
    if (dirty_)
        {
        ++code_;
        dirty_ = false;
        }
    }

bool TrackedObject::Token::dirty() const
    {
    return dirty_;
    }

void TrackedObject::Token::stage()
    {
    dirty_ = true;
    }

TrackedObject::Token::operator bool() const
    {
    return (id_ != invalid_id);
    }

bool TrackedObject::Token::operator==(const TrackedObject::Token& other) const
    {
    if (id_ == invalid_id || other.id_ == invalid_id)
        {
        throw std::runtime_error("Cannot compare DataTokens with invalid ids");
        }
    if (dirty_ || other.dirty_)
        {
        throw std::runtime_error("Cannot compare DataTokens without commiting changes");
        }
    return (id_ == other.id_ && code_ == other.code_);
    }

bool TrackedObject::Token::operator!=(const TrackedObject::Token& other) const
    {
    return !(*this == other);
    }

void TrackedObject::Dependencies::add(TrackedObject* object)
    {
    auto it = objects_.find(object->id());
    if (it == objects_.end())
        {
        objects_[object->id()] = object;
        }
    }

void TrackedObject::Dependencies::remove(Identifier id)
    {
    objects_.erase(id);
    tokens_.erase(id);
    }

void TrackedObject::Dependencies::clear()
    {
    objects_.clear();
    tokens_.clear();
    }

void TrackedObject::Dependencies::capture()
    {
    tokens_.clear();
    for (const auto& o : objects_)
        {
        tokens_[o.first] = o.second->token();
        }
    }

bool TrackedObject::Dependencies::changed()
    {
    bool result = false;
    for (const auto& o : objects_)
        {
        const auto tok = tokens_.find(o.first);
        if (tok == tokens_.end() || tok->second != o.second->token())
            {
            result = true;
            break;
            }
        }
    return result;
    }

}
