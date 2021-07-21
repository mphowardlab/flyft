#ifndef FLYFT_FIXED_POINT_ALGORITHM_MIXIN_H_
#define FLYFT_FIXED_POINT_ALGORITHM_MIXIN_H_

#include "iterative_algorithm_mixin.h"

class FixedPointAlgorithmMixin : public IterativeAlgorithmMixin
    {
    public:
        FixedPointAlgorithmMixin(double mix_param, int max_iterations, double tolerance)
            : IterativeAlgorithmMixin(max_iterations,tolerance)
            {
            setMixParameter(mix_param);
            }

        double getMixParameter() const
            {
            return mix_param_;
            }

        void setMixParameter(double mix_param)
            {
            if (mix_param <= 0 || mix_param > 1.0)
                {
                // error: mix parameter must be between 0 and 1
                }
            mix_param_ = mix_param;
            }

    protected:
        double mix_param_;
    };

#endif // FLYFT_FIXED_POINT_ALGORITHM_MIXIN_H_
