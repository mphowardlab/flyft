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

                bool operator==(const Token& other) const;
                bool operator!=(const Token& other) const;

            private:
                Identifier id_;
                uint64_t code_;
                bool dirty_;
            };

        class Dependencies
            {
            public:
                Dependencies() = default;
                Dependencies(std::shared_ptr<TrackedObject> object);
                Dependencies(const std::vector<std::shared_ptr<TrackedObject>>& objects);
                ~Dependencies() = default;

                bool changed();
                void capture();

                void add(std::shared_ptr<TrackedObject> object);
                void add(const std::vector<std::shared_ptr<TrackedObject>>& objects);
                void clear();

            private:
                std::unordered_map<Identifier,std::weak_ptr<TrackedObject>> objects_;
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
        Dependencies depends_;
        static Identifier count;
    };

}

#endif // FLYFT_TRACKED_OBJECT_H_
