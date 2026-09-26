#include "ForgeSim/core/ConstantComputation.hpp"


ConstantComputation::ConstantComputation(
    double value
)
    : value(value)
{
}


Result ConstantComputation::calculate(
    const std::vector<Result>&
) const
{
    return Result(value);
}


void ConstantComputation::setValue(
    double newValue
)
{
    value = newValue;
}


double ConstantComputation::getValue() const
{
    return value;
}