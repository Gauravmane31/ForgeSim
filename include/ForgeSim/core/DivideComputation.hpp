#pragma once

#include "ForgeSim/core/Computation.hpp"

class DivideComputation : public Computation {
public:
    Result calculate(
        const std::vector<Result>& inputs
    ) const override;
};