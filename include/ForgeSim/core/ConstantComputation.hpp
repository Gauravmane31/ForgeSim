#pragma once

#include "ForgeSim/core/Computation.hpp"


class ConstantComputation : public Computation
{
private:

    double value;


public:

    explicit ConstantComputation(
        double value
    );


    Result calculate(
        const std::vector<Result>& inputs
    ) const override;


    void setValue(
        double newValue
    );


    double getValue() const;
};