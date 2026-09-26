#pragma once

#include "ForgeSim/core/Computation.hpp"

class MultiplyComputation : public Computation {
public:
    Result calculate(
        const std::vector<Result>& inputs
    ) const override;
};