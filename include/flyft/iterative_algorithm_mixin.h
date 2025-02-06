#ifndef FLYFT_ITERATIVE_ALGORITHM_MIXIN_H_
#define FLYFT_ITERATIVE_ALGORITHM_MIXIN_H_

class IterativeAlgorithmMixin
    {
    public:
    IterativeAlgorithmMixin(int max_iterations, double tolerance)
        {
        setMaxIterations(max_iterations);
        setTolerance(tolerance);
        }

    int getMaxIterations() const
        {
        return max_iterations_;
        }

    void setMaxIterations(int max_iterations)
        {
        if (max_iterations < 1)
            {
            // error: max iterations must be at least 1
            }
        max_iterations_ = max_iterations;
        }

    double getTolerance() const
        {
        return tolerance_;
        }

    void setTolerance(double tolerance)
        {
        if (tolerance <= 0)
            {
            // error: tolerance must be positive
            }
        tolerance_ = tolerance;
        }

    protected:
    int max_iterations_;
    double tolerance_;
    };

#endif // FLYFT_ITERATIVE_ALGORITHM_MIXIN_H_
