#ifndef FLYFT_COMMUNICATOR_H_
#define FLYFT_COMMUNICATOR_H_

#include "flyft/field.h"

#include <memory>

namespace flyft
{

class Communicator
    {
    public:
        Communicator();
        virtual ~Communicator();

        void sync(std::shared_ptr<Field> field);
    };

}

#endif // FLYFT_COMMUNICATOR_H_
