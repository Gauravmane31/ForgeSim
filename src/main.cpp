#include <iomanip>
#include <iostream>
#include <memory>

#include "ForgeSim/core/ConstantComputation.hpp"
#include "ForgeSim/core/DivideComputation.hpp"
#include "ForgeSim/core/Graph.hpp"
#include "ForgeSim/core/MultiplyComputation.hpp"
#include "ForgeSim/execution/Scheduler.hpp"


void printResults(
    const std::unordered_map<int, Result>& results
)
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
    const ExecutionMetrics& metrics
)
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


int main()
{
    // ==================================================
    // CREATE GRAPH
    // ==================================================

    Graph graph;


    graph.addNode(
        Node(1, "Engine RPM")
    );

    graph.addNode(
        Node(2, "Gear Ratio")
    );

    graph.addNode(
        Node(3, "Engine Torque")
    );

    graph.addNode(
        Node(4, "Output RPM")
    );

    graph.addNode(
        Node(5, "Output Torque")
    );


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
            ConstantComputation
        >(3000.0);


    auto gearRatio =
        std::make_shared<
            ConstantComputation
        >(4.0);


    auto engineTorque =
        std::make_shared<
            ConstantComputation
        >(200.0);


    auto outputRpm =
        std::make_shared<
            DivideComputation
        >();


    auto outputTorque =
        std::make_shared<
            MultiplyComputation
        >();


    // ==================================================
    // REGISTER COMPUTATIONS
    // ==================================================

    Scheduler scheduler;


    scheduler.registerComputation(
        1,
        engineRpm
    );

    scheduler.registerComputation(
        2,
        gearRatio
    );

    scheduler.registerComputation(
        3,
        engineTorque
    );

    scheduler.registerComputation(
        4,
        outputRpm
    );

    scheduler.registerComputation(
        5,
        outputTorque
    );


    // ==================================================
    // FIRST EXECUTION
    // ==================================================

    std::cout
        << "\n============================================\n";

    std::cout
        << "        FIRST FULL EXECUTION\n";

    std::cout
        << "============================================\n";


    auto firstResults =
        scheduler.executeIncremental(
            graph,
            {1, 2, 3},
            4
        );


    printResults(firstResults);

    printMetrics(
        scheduler.getMetrics()
    );


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

    auto incrementalResults =
        scheduler.executeIncremental(
            graph,
            {1},
            4
        );


    printResults(
        incrementalResults
    );


    printMetrics(
        scheduler.getMetrics()
    );


    // ==================================================
    // END
    // ==================================================

    std::cout
        << "\n============================================\n";

    std::cout
        << "        INCREMENTAL EXECUTION COMPLETE\n";

    std::cout
        << "============================================\n";


    return 0;
}