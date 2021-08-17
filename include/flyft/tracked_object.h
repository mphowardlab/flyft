#ifndef FLYFT_TRACKED_OBJECT_H_
#define FLYFT_TRACKED_OBJECT_H_

#include <cstdint>

namespace flyft
{

class TrackedObject
    {
    public:
        using Identifier = uint64_t;
        class Token
            {
            public:
                Token();
                explicit Token(Identifier id);
                Token(Identifier id, uint64_t code);

                Token(const Token& other) = default;
                Token(Token&& other) = default;
                Token& operator=(const Token& other) = default;
                Token& operator=(Token&& other) = default;

                ~Token();

                void commit();
                bool dirty() const;
                void stage();

                bool operator==(const Token& other) const;
                bool operator!=(const Token& other) const;

            private:
                Identifier id_;
                uint64_t code_;
                bool dirty_;
            };

        TrackedObject();
        TrackedObject(const TrackedObject& other);
        TrackedObject(TrackedObject&& other);
        TrackedObject& operator=(const TrackedObject& other);
        TrackedObject& operator=(TrackedObject&& other);
        virtual ~TrackedObject();

        Identifier id() const;
        virtual const Token& token();

    protected:
        Identifier id_;
        Token token_;
        static Identifier count;
    };

}

#endif // FLYFT_TRACKED_OBJECT_H_
