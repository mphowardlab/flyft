#ifndef FLYFT_PARAMETER_H_
#define FLYFT_PARAMETER_H_

#include "flyft/state.h"

#include <memory>

namespace flyft
{

template<typename T>
class Parameter
    {
    public:
        Parameter()
            {
            }

        virtual ~Parameter()
            {
            }

        virtual T operator()(std::shared_ptr<State> state) = 0;

        T evaluate(std::shared_ptr<State> state)
            {
            return (*this)(state);
            }
    };
using DoubleParameter = Parameter<double>;

template<typename T>
class ConstantParameter : public Parameter<T>
    {
    public:
        ConstantParameter(T value)
            : value_(value)
            {
            }

        T operator()(std::shared_ptr<State> /*state*/) override
            {
            return value_;
            }

        T getValue() const
            {
            return value_;
            }

        void setValue(T value)
            {
            value_ = value;
            }

    private:
        T value_;
    };
using ConstantDoubleParameter = ConstantParameter<double>;

class LinearParameter : public DoubleParameter
    {
    public:
        LinearParameter(double initial, double origin, double rate)
            : initial_(initial), origin_(origin), rate_(rate)
            {
            }

        double operator()(std::shared_ptr<State> state) override
            {
            return initial_+rate_*(state->getTime()-origin_);
            }

        double getInitial() const
            {
            return initial_;
            }

        void setInitial(double initial)
            {
            initial_ = initial;
            }

        double getOrigin() const
            {
            return origin_;
            }

        void setOrigin(double origin)
            {
            origin_ = origin;
            }

        double getRate() const
            {
            return rate_;
            }

        void setRate(double rate)
            {
            rate_ = rate;
            }

    private:
        double initial_;
        double origin_;
        double rate_;
    };

}

#endif // FLYFT_PARAMETER_H_
