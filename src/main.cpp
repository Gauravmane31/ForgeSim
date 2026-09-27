#include <iomanip>
#include <iostream>
#include <memory>

#include "ForgeSim/core/ConstantComputation.hpp"
#include "ForgeSim/core/DivideComputation.hpp"
#include "ForgeSim/core/Graph.hpp"
#include "ForgeSim/core/MultiplyComputation.hpp"
#include "ForgeSim/execution/Scheduler.hpp"
#include "ForgeSim/core/BenchmarkGraph.hpp"
#include "ForgeSim/core/WorkComputation.hpp"
#include "ForgeSim/execution/Benchmark.hpp"

void printResults(
    const std::unordered_map<int, Result> &results)
{
    std::cout << "\n";

    std::cout
        << "Engine RPM     : "
        << results.at(1).getValue()
        << '\n';

    std::cout
        << "Gear Ratio     : "
        << results.at(2).getValue()
        << '\n';

    std::cout
        << "Engine Torque  : "
        << results.at(3).getValue()
        << '\n';

    std::cout
        << "Output RPM     : "
        << results.at(4).getValue()
        << '\n';

    std::cout
        << "Output Torque  : "
        << results.at(5).getValue()
        << '\n';
}

void printMetrics(
    const ExecutionMetrics &metrics)
{
    std::cout << "\n";
    std::cout
        << "Execution Metrics\n";

    std::cout
        << "-----------------\n";

    std::cout
        << "Total Nodes   : "
        << metrics.getTotalNodes()
        << '\n';

    std::cout
        << "Computed      : "
        << metrics.getComputedNodes()
        << '\n';

    std::cout
        << "Cached        : "
        << metrics.getCachedNodes()
        << '\n';

    std::cout
        << "Cache Hit Rate: "
        << std::fixed
        << std::setprecision(1)
        << metrics.getCacheHitRate()
        << "%\n";
}

void runBenchmark()
{
    constexpr std::size_t NODE_COUNT = 1000;
    constexpr std::size_t BRANCH_FACTOR = 2;
    constexpr std::size_t WORK_ITERATIONS = 50000;
    constexpr std::size_t WORKER_COUNT = 8;

    std::cout << "\n";
    std::cout << "============================================\n";
    std::cout << "           FORGESIM BENCHMARK\n";
    std::cout << "============================================\n";

    std::cout << "Nodes         : " << NODE_COUNT << "\n";
    std::cout << "Branch Factor : " << BRANCH_FACTOR << "\n";
    std::cout << "Workers       : " << WORKER_COUNT << "\n";
    std::cout << "Work/Node     : " << WORK_ITERATIONS << "\n\n";

    // ------------------------------------------------
    // Create benchmark graph
    // ------------------------------------------------

    Graph graph =
        BenchmarkGraph::create(
            NODE_COUNT,
            BRANCH_FACTOR
        );

    Scheduler scheduler;

    // Register computation for every node.
    for (std::size_t i = 1;
         i <= NODE_COUNT;
         ++i)
    {
        scheduler.registerComputation(
            static_cast<int>(i),
            std::make_shared<WorkComputation>(
                WORK_ITERATIONS
            )
        );
    }

    // ------------------------------------------------
    // Sequential full execution
    // ------------------------------------------------

    Benchmark sequentialTimer;

    auto sequentialResults =
        scheduler.execute(graph);

    const double sequentialTime =
        sequentialTimer.stopMilliseconds();

    std::cout << "--------------------------------------------\n";
    std::cout << "Sequential Full Execution\n";
    std::cout << "--------------------------------------------\n";

    std::cout << "Time      : "
              << Benchmark::formatMilliseconds(
                     sequentialTime
                 )
              << "\n";

    std::cout << "Computed  : "
              << sequentialResults.size()
              << "\n\n";

    // ------------------------------------------------
    // Parallel full execution
    // ------------------------------------------------

    Benchmark parallelTimer;

    auto parallelResults =
        scheduler.executeParallel(
            graph,
            WORKER_COUNT
        );

    const double parallelTime =
        parallelTimer.stopMilliseconds();

    std::cout << "--------------------------------------------\n";
    std::cout << "Parallel Full Execution\n";
    std::cout << "--------------------------------------------\n";

    std::cout << "Time      : "
              << Benchmark::formatMilliseconds(
                     parallelTime
                 )
              << "\n";

    std::cout << "Computed  : "
              << parallelResults.size()
              << "\n";

    if (parallelTime > 0.0)
    {
        const double speedup =
            sequentialTime / parallelTime;

        std::cout << "Speedup    : "
                  << std::fixed
                  << std::setprecision(2)
                  << speedup
                  << "x\n";
    }

    std::cout << "\n";

    // ------------------------------------------------
    // Prepare cache for incremental execution
    // ------------------------------------------------

    /*
        The incremental scheduler maintains its own cache.

        We mark every node dirty for the initial
        incremental execution.
    */

    for (std::size_t i = 1;
         i <= NODE_COUNT;
         ++i)
    {
        graph.markChanged(
            static_cast<int>(i)
        );
    }

    // ------------------------------------------------
    // Initial incremental execution
    // ------------------------------------------------

    Benchmark initialIncrementalTimer;

    auto initialIncrementalResults =
        scheduler.executeIncremental(
            graph,
            WORKER_COUNT
        );

    const double initialIncrementalTime =
        initialIncrementalTimer.stopMilliseconds();

    std::cout << "--------------------------------------------\n";
    std::cout << "Initial Incremental Execution\n";
    std::cout << "--------------------------------------------\n";

    std::cout << "Time      : "
              << Benchmark::formatMilliseconds(
                     initialIncrementalTime
                 )
              << "\n";

    std::cout << "Computed  : "
              << scheduler.getMetrics().getComputedNodes()
              << "\n";

    std::cout << "Cached    : "
              << scheduler.getMetrics().getCachedNodes()
              << "\n\n";

    // ------------------------------------------------
    // Change one middle node
    // ------------------------------------------------

    const int changedNode =
        static_cast<int>(NODE_COUNT / 2);

    graph.markChanged(changedNode);

    // ------------------------------------------------
    // Incremental execution after change
    // ------------------------------------------------

    Benchmark incrementalTimer;

    auto incrementalResults =
        scheduler.executeIncremental(
            graph,
            WORKER_COUNT
        );

    const double incrementalTime =
        incrementalTimer.stopMilliseconds();

    const auto& incrementalMetrics =
        scheduler.getMetrics();

    std::cout << "--------------------------------------------\n";
    std::cout << "Incremental Execution After Change\n";
    std::cout << "--------------------------------------------\n";

    std::cout << "Changed Node : "
              << changedNode
              << "\n";

    std::cout << "Time         : "
              << Benchmark::formatMilliseconds(
                     incrementalTime
                 )
              << "\n";

    std::cout << "Total Nodes  : "
              << incrementalMetrics.getTotalNodes()
              << "\n";

    std::cout << "Computed     : "
              << incrementalMetrics.getComputedNodes()
              << "\n";

    std::cout << "Cached       : "
              << incrementalMetrics.getCachedNodes()
              << "\n";

    std::cout << "Cache Hit    : "
              << std::fixed
              << std::setprecision(2)
              << incrementalMetrics.getCacheHitRate()
              << "%\n\n";

    // ------------------------------------------------
    // No-change execution
    // ------------------------------------------------

    Benchmark noChangeTimer;

    auto noChangeResults =
        scheduler.executeIncremental(
            graph,
            WORKER_COUNT
        );

    const double noChangeTime =
        noChangeTimer.stopMilliseconds();

    const auto& noChangeMetrics =
        scheduler.getMetrics();

    std::cout << "--------------------------------------------\n";
    std::cout << "No-Change Execution\n";
    std::cout << "--------------------------------------------\n";

    std::cout << "Time         : "
              << Benchmark::formatMilliseconds(
                     noChangeTime
                 )
              << "\n";

    std::cout << "Computed     : "
              << noChangeMetrics.getComputedNodes()
              << "\n";

    std::cout << "Cached       : "
              << noChangeMetrics.getCachedNodes()
              << "\n";

    std::cout << "Cache Hit    : "
              << std::fixed
              << std::setprecision(2)
              << noChangeMetrics.getCacheHitRate()
              << "%\n\n";

    // ------------------------------------------------
    // Summary
    // ------------------------------------------------

    std::cout << "============================================\n";
    std::cout << "             BENCHMARK SUMMARY\n";
    std::cout << "============================================\n";

    std::cout << "Sequential Time : "
              << Benchmark::formatMilliseconds(
                     sequentialTime
                 )
              << "\n";

    std::cout << "Parallel Time   : "
              << Benchmark::formatMilliseconds(
                     parallelTime
                 )
              << "\n";

    std::cout << "Incremental Time: "
              << Benchmark::formatMilliseconds(
                     incrementalTime
                 )
              << "\n";

    std::cout << "No-Change Time  : "
              << Benchmark::formatMilliseconds(
                     noChangeTime
                 )
              << "\n";

    std::cout << "============================================\n";
}

int main()
{
    // ==================================================
    // CREATE GRAPH
    // ==================================================

    Graph graph;

    graph.addNode(
        Node(1, "Engine RPM"));

    graph.addNode(
        Node(2, "Gear Ratio"));

    graph.addNode(
        Node(3, "Engine Torque"));

    graph.addNode(
        Node(4, "Output RPM"));

    graph.addNode(
        Node(5, "Output Torque"));

    // ==================================================
    // DEFINE DEPENDENCIES
    // ==================================================

    graph.addDependency(4, 1);
    graph.addDependency(4, 2);

    graph.addDependency(5, 2);
    graph.addDependency(5, 3);

    // ==================================================
    // CREATE COMPUTATIONS
    // ==================================================

    auto engineRpm =
        std::make_shared<
            ConstantComputation>(3000.0);

    auto gearRatio =
        std::make_shared<
            ConstantComputation>(4.0);

    auto engineTorque =
        std::make_shared<
            ConstantComputation>(200.0);

    auto outputRpm =
        std::make_shared<
            DivideComputation>();

    auto outputTorque =
        std::make_shared<
            MultiplyComputation>();

    // ==================================================
    // REGISTER COMPUTATIONS
    // ==================================================

    Scheduler scheduler;

    scheduler.registerComputation(
        1,
        engineRpm);

    scheduler.registerComputation(
        2,
        gearRatio);

    scheduler.registerComputation(
        3,
        engineTorque);

    scheduler.registerComputation(
        4,
        outputRpm);

    scheduler.registerComputation(
        5,
        outputTorque);

    // ==================================================
    // FIRST EXECUTION
    // ==================================================

    std::cout
        << "\n============================================\n";

    std::cout
        << "        FIRST FULL EXECUTION\n";

    std::cout
        << "============================================\n";

    graph.markChanged(1);
    graph.markChanged(2);
    graph.markChanged(3);

    auto firstResults =
        scheduler.executeIncremental(
            graph,
            4);

    printResults(firstResults);

    printMetrics(
        scheduler.getMetrics());

    // ==================================================
    // CHANGE ENGINE RPM
    // ==================================================

    std::cout
        << "\n============================================\n";

    std::cout
        << "       CHANGE: ENGINE RPM\n";

    std::cout
        << "============================================\n";

    std::cout
        << "Engine RPM: 3000 -> 3200\n";

    engineRpm->setValue(3200.0);

    // ==================================================
    // INCREMENTAL EXECUTION
    // ==================================================

    engineRpm->setValue(3200.0);

    graph.markChanged(1);

    auto incrementalResults =
        scheduler.executeIncremental(
            graph,
            4);

    printResults(
        incrementalResults);

    printMetrics(
        scheduler.getMetrics());

    // ==================================================
    // END
    // ==================================================

    std::cout
        << "\n============================================\n";

    std::cout
        << "        INCREMENTAL EXECUTION COMPLETE\n";

    std::cout
        << "============================================\n";

    auto noChangeResults =
    scheduler.executeIncremental(
        graph,
        4
    );

    printResults(noChangeResults);
    printMetrics(scheduler.getMetrics());

    std::cout
        << "\n============================================\n";

    std::cout
        << "        NO CHANGE EXECUTION COMPLETE\n";

    std::cout
        << "============================================\n";

    runBenchmark();
    return 0;
}