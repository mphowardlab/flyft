#ifndef FLYFT_DATA_TOKEN_H_
#define FLYFT_DATA_TOKEN_H_

#include <cstdint>

namespace flyft
{

class DataToken
    {
    public:
        using Type = uint64_t;

        DataToken();
        explicit DataToken(Type id);
        DataToken(Type id, Type code);

        void advance();

        bool operator==(const DataToken& other) const;
        bool operator!=(const DataToken& other) const;

    private:
        Type id_;
        Type code_;
    };

}

#endif // FLYFT_DATA_TOKEN_H_
