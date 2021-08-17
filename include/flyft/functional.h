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

        virtual void compute(std::shared_ptr<State> state) = 0;

        double getValue() const;

        const TypeMap<std::shared_ptr<Field>>& getDerivatives();
        std::shared_ptr<Field> getDerivative(const std::string& type);
        std::shared_ptr<const Field> getDerivative(const std::string& type) const;
        virtual void requestDerivativeBuffer(const std::string& type, int buffer_request);

        virtual int determineBufferShape(std::shared_ptr<State> state, const std::string& type);

    protected:
        double value_;
        TypeMap<std::shared_ptr<Field>> derivatives_;
        TypeMap<int> buffer_requests_;
        State::Token state_token_;

        virtual void setup(std::shared_ptr<State> state);
        virtual bool needsCompute(std::shared_ptr<State> state);
    };

}

#endif // FLYFT_FUNCTIONAL_H_
