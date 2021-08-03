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
    const auto f = field->view();
    const int shape = field->shape();
    const int buffer_shape = field->buffer_shape();
    for (int idx=0; idx < buffer_shape; ++idx)
        {
        f(shape+idx) = f(idx);
        f(-1-idx) = f(shape-1-idx);
        }
    }

}
