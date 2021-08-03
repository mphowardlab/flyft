#include "flyft/communicator.h"

namespace flyft
{

Communicator::Communicator()
    {
    }

Communicator::~Communicator()
    {
    }

void Communicator::sync(std::shared_ptr<Field> field)
    {
    // check if field was recently synced and stop if not needed
    auto token = field_tokens_.find(field->id());
    if (token != field_tokens_.end() && token->second == field->token())
        {
        return;
        }

    // sync field
    const auto f = field->view();
    const int shape = field->shape();
    const int buffer_shape = field->buffer_shape();
    for (int idx=0; idx < buffer_shape; ++idx)
        {
        f(shape+idx) = f(idx);
        f(-1-idx) = f(shape-1-idx);
        }

    // cache token
    field_tokens_[field->id()] = field->token();
    }

}
