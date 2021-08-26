#ifndef FLYFT_FUNCTIONAL_H_
#define FLYFT_FUNCTIONAL_H_

#include "flyft/field.h"
#include "flyft/state.h"
#include "flyft/tracked_object.h"
#include "flyft/type_map.h"

#include <memory>
#include <string>

namespace flyft
{

class Functional : public TrackedObject
    {
    public:
        Functional();
        virtual ~Functional();

        // noncopyable / nonmovable
        Functional(const Functional&) = delete;
        Functional(Functional&&) = delete;
        Functional& operator=(const Functional&) = delete;
        Functional& operator=(Functional&&) = delete;

        virtual Token compute(std::shared_ptr<State> state, bool compute_value);

        double getValue() const;

        const TypeMap<std::shared_ptr<Field>>& getDerivatives();
        std::shared_ptr<Field> getDerivative(const std::string& type);
        std::shared_ptr<const Field> getDerivative(const std::string& type) const;
        virtual void requestDerivativeBuffer(const std::string& type, int buffer_request);

        bool needsBuffer(std::shared_ptr<State> state);
        virtual int determineBufferShape(std::shared_ptr<State> state, const std::string& type);

    protected:
        double value_;
        TypeMap<std::shared_ptr<Field>> derivatives_;
        TypeMap<int> buffer_requests_;
        Dependencies compute_depends_;
        Token compute_token_;
        Token compute_state_token_;

        virtual bool setup(std::shared_ptr<State> state, bool compute_value);
        virtual void _compute(std::shared_ptr<State> state, bool compute_value) = 0;
        virtual Token finalize(std::shared_ptr<State> state, bool compute_value);
    };

}

#endif // FLYFT_FUNCTIONAL_H_
