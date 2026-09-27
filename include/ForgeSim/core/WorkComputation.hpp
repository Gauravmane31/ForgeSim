#pragma once

#include "ForgeSim/core/Computation.hpp"

class WorkComputation : public Computation
{
private:
    std::size_t iterations;

public:
    explicit WorkComputation(
        std::size_t iterations = 100000
    );

    Result calculate(
        const std::vector<Result>& inputs
    ) const override;
};