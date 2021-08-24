#ifndef FLYFT_TRACKED_OBJECT_H_
#define FLYFT_TRACKED_OBJECT_H_

#include <cstdint>
#include <memory>
#include <unordered_map>
#include <vector>

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

                operator bool() const;
                bool operator==(const Token& other) const;
                bool operator!=(const Token& other) const;

            private:
                Identifier id_;
                uint64_t code_;
                bool dirty_;

                static const Identifier invalid_id = 0xffffffffffffffffu;
            };

        class Dependencies
            {
            public:
                Dependencies() = default;
                ~Dependencies() = default;

                void add(TrackedObject* object);
                void remove(Identifier id);
                void clear();
                bool contains(Identifier id) const;

                bool changed();
                void capture();

            private:
                std::unordered_map<Identifier,TrackedObject*> objects_;
                std::unordered_map<Identifier,Token> tokens_;
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
