#include "ForgeSim/core/BenchmarkGraph.hpp"

#include <stdexcept>

Graph BenchmarkGraph::create(
    std::size_t nodeCount,
    std::size_t
)
{
    if (nodeCount < 2)
    {
        throw std::invalid_argument(
            "Benchmark graph must contain at least 2 nodes"
        );
    }

    Graph graph;

    // ------------------------------------------------
    // Create all nodes
    // ------------------------------------------------

    for (std::size_t i = 0; i < nodeCount; ++i)
    {
        graph.addNode(
            Node(
                static_cast<int>(i + 1),
                "Benchmark Node " +
                std::to_string(i + 1)
            )
        );
    }

    /*
        Benchmark DAG structure:

                         ┌──► Node 2 ──┐
                         ├──► Node 3 ──┤
                         ├──► Node 4 ──┤
                         ├──► Node 5 ──┤
        Node 1 ──────────┼──► Node 6 ──┼──► Final Node
                         ├──► Node 7 ──┤
                         ├──► Node 8 ──┤
                         └──► Node 9 ──┘

        Node 1 is the root.

        Nodes 2 ... N-1 are independent after Node 1
        completes and can therefore execute in parallel.

        Node N is the final aggregation node and depends
        on all middle nodes.

        This structure intentionally exposes substantial
        parallelism to the scheduler.
    */

    const int rootNode = 1;
    const int finalNode =
        static_cast<int>(nodeCount);

    // ------------------------------------------------
    // Root -> independent middle nodes
    // ------------------------------------------------

    for (int nodeId = 2;
         nodeId < finalNode;
         ++nodeId)
    {
        graph.addDependency(
            nodeId,
            rootNode
        );
    }

    // ------------------------------------------------
    // Independent middle nodes -> final node
    // ------------------------------------------------

    for (int nodeId = 2;
         nodeId < finalNode;
         ++nodeId)
    {
        graph.addDependency(
            finalNode,
            nodeId
        );
    }

    return graph;
}