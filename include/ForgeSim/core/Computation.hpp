#pragma once

#include <vector>

#include "ForgeSim/core/Result.hpp"

class Computation {
public:
    virtual ~Computation() = default;

    virtual Result calculate(
        const std::vector<Result>& inputs
    ) const = 0;
};