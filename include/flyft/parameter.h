#ifndef FLYFT_PARAMETER_H_
#define FLYFT_PARAMETER_H_

#include "flyft/state.h"
#include "flyft/tracked_object.h"

#include <cmath>
#include <memory>

namespace flyft
{

template<typename T>
class Parameter : public TrackedObject
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
            if (this->token_.dirty())
                {
                this->token_.commit();
                }
            return value_;
            }

        T getValue() const
            {
            return value_;
            }

        void setValue(T value)
            {
            value_ = value;
            this->token_.stage();
            }

    private:
        T value_;
    };
using ConstantDoubleParameter = ConstantParameter<double>;

class LinearParameter : public DoubleParameter
    {
    public:
        LinearParameter(double initial, double origin, double rate)
            : initial_(initial), origin_(origin), rate_(rate), time_(std::nan("")), value_(std::nan(""))
            {
            }

        double operator()(std::shared_ptr<State> state) override
            {
            // record time for evaluation
            const double time = state->getTime();
            if (time != time_)
                {
                time_ = time;
                this->token_.stage();
                }

            // if anything has changed, evaluate and store
            if (this->token_.dirty())
                {
                value_ = initial_+rate_*(time_-origin_);
                this->token_.commit();
                }
            return value_;
            }

        double getInitial() const
            {
            return initial_;
            }

        void setInitial(double initial)
            {
            initial_ = initial;
            this->token_.stage();
            }

        double getOrigin() const
            {
            return origin_;
            }

        void setOrigin(double origin)
            {
            origin_ = origin;
            this->token_.stage();
            }

        double getRate() const
            {
            return rate_;
            }

        void setRate(double rate)
            {
            rate_ = rate;
            this->token_.stage();
            }

    private:
        double initial_;
        double origin_;
        double rate_;
        double time_;
        double value_;
    };
    
class SquareRootParameter : public DoubleParameter
    {
    public:
        SquareRootParameter(double initial, double origin, double rate)
            : initial_(initial), origin_(origin), rate_(rate), time_(std::nan("")), value_(std::nan(""))
            {
            }

        double operator()(std::shared_ptr<State> state) override
            {
            // record time for evaluation
            const double time = state->getTime();
            if (time != time_)
                {
                time_ = time;
                this->token_.stage();
                }

            // if anything has changed, evaluate and store
            if (this->token_.dirty())
                {
                const double value_sq = initial_*initial_+rate_*(time_-origin_);
                if(value_sq < 0)
                    {
                    throw std::invalid_argument("Time makes argument of square root negative");
                    }
                value_ = std::sqrt(value_sq);
                this->token_.commit();
                }
            return value_;
            }

        double getInitial() const
            {
            return initial_;
            }

        void setInitial(double initial)
            {
            initial_ = initial;
            this->token_.stage();
            }

        double getOrigin() const
            {
            return origin_;
            }

        void setOrigin(double origin)
            {
            origin_ = origin;
            this->token_.stage();
            }

        double getRate() const
            {
            return rate_;
            }

        void setRate(double rate)
            {
            rate_ = rate;
            this->token_.stage();
            }
    private:
        double initial_;
        double origin_;
        double rate_;
        double time_;
        double value_;
    };

}
#endif // FLYFT_PARAMETER_H_
