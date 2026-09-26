#include "ForgeSim/core/DivideComputation.hpp"

#include <stdexcept>

Result DivideComputation::calculate(
    const std::vector<Result>& inputs
) const {

    if (inputs.size() != 2) {
        throw std::runtime_error(
            "DivideComputation requires exactly two inputs"
        );
    }

    if (!inputs[0].isValid() || !inputs[1].isValid()) {
        throw std::runtime_error(
            "Cannot divide invalid result"
        );
    }

    if (inputs[1].getValue() == 0.0) {
        throw std::runtime_error(
            "Division by zero"
        );
    }

    return Result(
        inputs[0].getValue() /
        inputs[1].getValue()
    );
}