#include "ForgeSim/core/WorkComputation.hpp"

#include <cmath>

WorkComputation::WorkComputation(
    std::size_t iterations
)
    : iterations(iterations)
{
}

Result WorkComputation::calculate(
    const std::vector<Result>& inputs
) const
{
    double value = 1.0;

    if (!inputs.empty())
    {
        for (const Result& input : inputs)
        {
            if (!input.isValid())
            {
                return Result();
            }

            value += input.getValue();
        }
    }

    double accumulator = value;

    for (std::size_t i = 0; i < iterations; ++i)
    {
        accumulator =
            std::sin(accumulator * 1.000001)
            + std::cos(accumulator * 0.999999);
    }

    return Result(accumulator);
}