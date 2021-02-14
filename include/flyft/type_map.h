#ifndef FLYFT_TYPE_MAP_H_
#define FLYFT_TYPE_MAP_H_

#include <string>
#include <unordered_map>

namespace flyft
{

template<typename T>
using TypeMap = std::unordered_map<std::string,T>;

}

#endif // FLYFT_TYPE_MAP_H_
