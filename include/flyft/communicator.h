#ifndef FLYFT_COMMUNICATOR_H_
#define FLYFT_COMMUNICATOR_H_

#include "flyft/data_token.h"
#include "flyft/field.h"

#include <memory>
#include <unordered_map>

namespace flyft
{

class Communicator
    {
    public:
        Communicator();
        virtual ~Communicator();

        void sync(std::shared_ptr<Field> field);

    private:
        std::unordered_map<Field::Identifier,DataToken> field_tokens_;
    };

}

#endif // FLYFT_COMMUNICATOR_H_
