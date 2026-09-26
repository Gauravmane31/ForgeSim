#include "ForgeSim/core/MultiplyComputation.hpp"

#include <stdexcept>

Result MultiplyComputation::calculate(
    const std::vector<Result>& inputs
) const {

    if (inputs.empty()) {
        throw std::runtime_error(
            "MultiplyComputation requires at least one input"
        );
    }

    double result = 1.0;

    for (const Result& input : inputs) {

        if (!input.isValid()) {
            throw std::runtime_error(
                "Cannot multiply invalid result"
            );
        }

        result *= input.getValue();
    }

    return Result(result);
}