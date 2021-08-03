#include "flyft/data_token.h"

namespace flyft
{

DataToken::DataToken()
    : DataToken(0,0)
    {}

DataToken::DataToken(Type id)
    : DataToken(id,0)
    {}

DataToken::DataToken(Type id, Type code)
    : id_(id), code_(code)
    {}

void DataToken::advance()
    {
    ++code_;
    }

bool DataToken::operator==(const DataToken& other) const
    {
    return (id_ == other.id_ && code_ == other.code_);
    }

bool DataToken::operator!=(const DataToken& other) const
    {
    return !(*this == other);
    }

}
