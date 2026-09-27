#include <gtest/gtest.h>

#include <memory>

#include "ForgeSim/core/ConstantComputation.hpp"
#include "ForgeSim/core/DivideComputation.hpp"
#include "ForgeSim/execution/Scheduler.hpp"

class SchedulerTest : public ::testing::Test
{
protected:

    std::shared_ptr<ConstantComputation> rpm;
    std::shared_ptr<ConstantComputation> ratio;

    Graph createGraph()
    {
        Graph graph;

        graph.addNode(Node(1, "RPM"));
        graph.addNode(Node(2, "Ratio"));
        graph.addNode(Node(3, "Output"));

        graph.addDependency(3, 1);
        graph.addDependency(3, 2);

        return graph;
    }

    Scheduler createScheduler()
    {
        Scheduler scheduler;

        rpm =
            std::make_shared<ConstantComputation>(
                3000.0
            );

        ratio =
            std::make_shared<ConstantComputation>(
                4.0
            );

        scheduler.registerComputation(
            1,
            rpm
        );

        scheduler.registerComputation(
            2,
            ratio
        );

        scheduler.registerComputation(
            3,
            std::make_shared<DivideComputation>()
        );

        return scheduler;
    }
};

TEST_F(
    SchedulerTest,
    SequentialExecutionProducesCorrectResult
)
{
    Graph graph = createGraph();

    Scheduler scheduler = createScheduler();

    auto results =
        scheduler.execute(graph);

    ASSERT_EQ(results.size(), 3);

    EXPECT_DOUBLE_EQ(
        results.at(1).getValue(),
        3000.0
    );

    EXPECT_DOUBLE_EQ(
        results.at(2).getValue(),
        4.0
    );

    EXPECT_DOUBLE_EQ(
        results.at(3).getValue(),
        750.0
    );
}

TEST_F(
    SchedulerTest,
    ParallelExecutionProducesCorrectResult
)
{
    Graph graph = createGraph();

    Scheduler scheduler = createScheduler();

    auto results =
        scheduler.executeParallel(
            graph,
            4
        );

    ASSERT_EQ(results.size(), 3);

    EXPECT_DOUBLE_EQ(
        results.at(3).getValue(),
        750.0
    );
}

TEST_F(
    SchedulerTest,
    IncrementalExecutionRecomputesOnlyAffectedNodes
)
{
    Graph graph = createGraph();

    Scheduler scheduler = createScheduler();

    // Initial computation.
    graph.markChanged(1);
    graph.markChanged(2);
    graph.markChanged(3);

    auto firstResults =
        scheduler.executeIncremental(
            graph,
            4
        );

    ASSERT_EQ(firstResults.size(), 3);

    EXPECT_EQ(
        scheduler.getMetrics().getComputedNodes(),
        3
    );

    // Change RPM.
    rpm->setValue(3200.0);

    graph.markChanged(1);

    auto secondResults =
        scheduler.executeIncremental(
            graph,
            4
        );

    ASSERT_EQ(secondResults.size(), 3);

    EXPECT_DOUBLE_EQ(
        secondResults.at(1).getValue(),
        3200.0
    );

    EXPECT_DOUBLE_EQ(
        secondResults.at(2).getValue(),
        4.0
    );

    EXPECT_DOUBLE_EQ(
        secondResults.at(3).getValue(),
        800.0
    );

    EXPECT_EQ(
        scheduler.getMetrics().getComputedNodes(),
        2
    );

    EXPECT_EQ(
        scheduler.getMetrics().getCachedNodes(),
        1
    );
}

TEST_F(
    SchedulerTest,
    NoChangeUsesCache
)
{
    Graph graph = createGraph();

    Scheduler scheduler = createScheduler();

    graph.markChanged(1);
    graph.markChanged(2);
    graph.markChanged(3);

    scheduler.executeIncremental(
        graph,
        4
    );

    auto results =
        scheduler.executeIncremental(
            graph,
            4
        );

    ASSERT_EQ(results.size(), 3);

    EXPECT_EQ(
        scheduler.getMetrics().getComputedNodes(),
        0
    );

    EXPECT_EQ(
        scheduler.getMetrics().getCachedNodes(),
        3
    );

    EXPECT_DOUBLE_EQ(
        scheduler.getMetrics().getCacheHitRate(),
        100.0
    );
}
