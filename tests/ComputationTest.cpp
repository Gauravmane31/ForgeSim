#include <gtest/gtest.h>

#include "ForgeSim/core/ConstantComputation.hpp"
#include "ForgeSim/core/MultiplyComputation.hpp"
#include "ForgeSim/core/DivideComputation.hpp"

// ============================================================
// ConstantComputation
// ============================================================

TEST(ConstantComputationTest, ReturnsConfiguredValue)
{
    ConstantComputation computation(42.0);

    Result result =
        computation.calculate({});

    ASSERT_TRUE(result.isValid());
    EXPECT_DOUBLE_EQ(result.getValue(), 42.0);
}

TEST(ConstantComputationTest, ValueCanBeChanged)
{
    ConstantComputation computation(10.0);

    computation.setValue(25.0);

    Result result =
        computation.calculate({});

    ASSERT_TRUE(result.isValid());
    EXPECT_DOUBLE_EQ(result.getValue(), 25.0);
}

// ============================================================
// MultiplyComputation
// ============================================================

TEST(MultiplyComputationTest, MultipliesInputs)
{
    MultiplyComputation computation;

    std::vector<Result> inputs{
        Result(2.0),
        Result(3.0),
        Result(4.0)
    };

    Result result =
        computation.calculate(inputs);

    ASSERT_TRUE(result.isValid());
    EXPECT_DOUBLE_EQ(result.getValue(), 24.0);
}

TEST(MultiplyComputationTest, EmptyInputThrows)
{
    MultiplyComputation computation;

    EXPECT_THROW(
        computation.calculate({}),
        std::runtime_error
    );
}

TEST(MultiplyComputationTest, InvalidInputThrows)
{
    MultiplyComputation computation;

    Result invalidResult;
    Result validResult(10.0);

    std::vector<Result> inputs{
        validResult,
        invalidResult
    };

    EXPECT_THROW(
        computation.calculate(inputs),
        std::runtime_error
    );
}

// ============================================================
// DivideComputation
// ============================================================

TEST(DivideComputationTest, DividesTwoInputs)
{
    DivideComputation computation;

    std::vector<Result> inputs{
        Result(20.0),
        Result(4.0)
    };

    Result result =
        computation.calculate(inputs);

    ASSERT_TRUE(result.isValid());
    EXPECT_DOUBLE_EQ(result.getValue(), 5.0);
}

TEST(DivideComputationTest, RequiresExactlyTwoInputs)
{
    DivideComputation computation;

    std::vector<Result> inputs{
        Result(20.0)
    };

    EXPECT_THROW(
        computation.calculate(inputs),
        std::runtime_error
    );
}

TEST(DivideComputationTest, DivisionByZeroThrows)
{
    DivideComputation computation;

    std::vector<Result> inputs{
        Result(20.0),
        Result(0.0)
    };

    EXPECT_THROW(
        computation.calculate(inputs),
        std::runtime_error
    );
}