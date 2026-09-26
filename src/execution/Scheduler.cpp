#include "ForgeSim/execution/Scheduler.hpp"

#include <queue>
#include <stdexcept>
#include <vector>


// ============================================================
// Register a computation for a node
// ============================================================

void Scheduler::registerComputation(
    int nodeId,
    std::shared_ptr<Computation> computation
)
{
    if (!computation)
    {
        throw std::invalid_argument(
            "Cannot register a null computation"
        );
    }

    computations[nodeId] =
        std::move(computation);
}


// ============================================================
// Sequential execution
// ============================================================

std::unordered_map<int, Result>
Scheduler::execute(
    const Graph& graph
) const
{
    std::unordered_map<int, Result> results;

    const std::vector<int> executionOrder =
        graph.topologicalSort();


    for (int nodeId : executionOrder)
    {
        const Node* node =
            graph.getNode(nodeId);


        if (node == nullptr)
        {
            throw std::runtime_error(
                "Node not found during execution"
            );
        }


        auto computationIt =
            computations.find(nodeId);


        if (computationIt ==
            computations.end())
        {
            throw std::runtime_error(
                "No computation registered for node: " +
                std::to_string(nodeId)
            );
        }


        std::vector<Result> inputs;


        for (int dependencyId :
             node->getDependencies())
        {
            auto resultIt =
                results.find(dependencyId);


            if (resultIt ==
                results.end())
            {
                throw std::runtime_error(
                    "Dependency result not available for node: " +
                    std::to_string(nodeId)
                );
            }


            inputs.push_back(
                resultIt->second
            );
        }


        Result result =
            computationIt->second->calculate(
                inputs
            );


        results[nodeId] =
            result;
    }


    return results;
}


// ============================================================
// Parallel execution
// ============================================================

std::unordered_map<int, Result>
Scheduler::executeParallel(
    const Graph& graph,
    std::size_t workerCount
) const
{
    ThreadPool pool(workerCount);

    std::unordered_map<int, Result>
        results;

    std::unordered_map<int, int>
        remainingDependencies;

    std::queue<int> ready;


    // --------------------------------------------------------
    // Find initially-ready nodes
    // --------------------------------------------------------

    for (const auto& [id, node] :
         graph.getNodes())
    {
        remainingDependencies[id] =
            static_cast<int>(
                node.getDependencies().size()
            );


        if (remainingDependencies[id] == 0)
        {
            ready.push(id);
        }
    }


    std::size_t processedNodes = 0;


    // --------------------------------------------------------
    // Process graph level by level
    // --------------------------------------------------------

    while (!ready.empty())
    {
        std::vector<int> currentBatch;


        while (!ready.empty())
        {
            currentBatch.push_back(
                ready.front()
            );

            ready.pop();
        }


        struct PendingTask
        {
            int nodeId;

            std::future<Result> future;
        };


        std::vector<PendingTask>
            pendingTasks;


        // ----------------------------------------------------
        // Submit current batch to ThreadPool
        // ----------------------------------------------------

        for (int nodeId :
             currentBatch)
        {
            const Node* node =
                graph.getNode(nodeId);


            if (node == nullptr)
            {
                throw std::runtime_error(
                    "Node not found during parallel execution"
                );
            }


            auto computationIt =
                computations.find(nodeId);


            if (computationIt ==
                computations.end())
            {
                throw std::runtime_error(
                    "No computation registered for node: " +
                    std::to_string(nodeId)
                );
            }


            std::vector<Result> inputs;


            for (int dependencyId :
                 node->getDependencies())
            {
                auto resultIt =
                    results.find(dependencyId);


                if (resultIt ==
                    results.end())
                {
                    throw std::runtime_error(
                        "Dependency result not available"
                    );
                }


                inputs.push_back(
                    resultIt->second
                );
            }


            auto future =
                pool.submit(
                    [
                        computation =
                            computationIt->second,

                        inputs =
                            std::move(inputs)
                    ]()
                    {
                        return computation->
                            calculate(inputs);
                    }
                );


            pendingTasks.push_back(
                {
                    nodeId,
                    std::move(future)
                }
            );
        }


        // ----------------------------------------------------
        // Collect results
        // ----------------------------------------------------

        for (auto& task :
             pendingTasks)
        {
            Result result =
                task.future.get();


            results[task.nodeId] =
                result;


            ++processedNodes;
        }


        // ----------------------------------------------------
        // Unlock dependent nodes
        // ----------------------------------------------------

        for (int completedNodeId :
             currentBatch)
        {
            const Node* node =
                graph.getNode(
                    completedNodeId
                );


            for (int dependentId :
                 node->getDependents())
            {
                --remainingDependencies[
                    dependentId
                ];


                if (
                    remainingDependencies[
                        dependentId
                    ] == 0
                )
                {
                    ready.push(
                        dependentId
                    );
                }
            }
        }
    }


    // --------------------------------------------------------
    // Detect invalid graph
    // --------------------------------------------------------

    if (
        processedNodes !=
        graph.getNodeCount()
    )
    {
        throw std::runtime_error(
            "Graph contains a cycle or invalid dependency state"
        );
    }


    return results;
}


// ============================================================
// Incremental execution
// ============================================================

std::unordered_map<int, Result>
Scheduler::executeIncremental(
    const Graph& graph,
    const std::vector<int>& changedNodes,
    std::size_t workerCount
)
{
    // --------------------------------------------------------
    // STEP 1:
    // Find every node affected by the change.
    // --------------------------------------------------------

    const auto affectedNodes =
        impactAnalyzer.findAffectedNodes(
            graph,
            changedNodes
        );


    // --------------------------------------------------------
    // STEP 2:
    // Reset metrics for this execution.
    // --------------------------------------------------------

    metrics.reset();


    metrics.setTotalNodes(
        graph.getNodeCount()
    );


    // --------------------------------------------------------
    // STEP 3:
    // Everything outside the affected subgraph
    // can potentially be reused from cache.
    // --------------------------------------------------------

    const std::size_t unaffectedNodes =
        graph.getNodeCount() -
        affectedNodes.size();


    for (
        std::size_t i = 0;
        i < unaffectedNodes;
        ++i
    )
    {
        metrics.recordCacheHit();
    }


    // --------------------------------------------------------
    // STEP 4:
    // Invalidate affected nodes.
    // --------------------------------------------------------

    for (int nodeId :
         affectedNodes)
    {
        cacheManager.invalidate(
            nodeId
        );
    }


    // --------------------------------------------------------
    // STEP 5:
    // Create worker pool.
    // --------------------------------------------------------

    ThreadPool pool(workerCount);


    std::unordered_map<int, int>
        remainingDependencies;

    std::queue<int> ready;


    // --------------------------------------------------------
    // STEP 6:
    // Calculate how many affected dependencies
    // each affected node is waiting for.
    // --------------------------------------------------------

    for (int nodeId :
         affectedNodes)
    {
        const Node* node =
            graph.getNode(nodeId);


        if (node == nullptr)
        {
            throw std::runtime_error(
                "Affected node not found"
            );
        }


        int remaining = 0;


        for (int dependencyId :
             node->getDependencies())
        {
            if (
                affectedNodes.contains(
                    dependencyId
                )
            )
            {
                ++remaining;
            }
        }


        remainingDependencies[nodeId] =
            remaining;


        if (remaining == 0)
        {
            ready.push(nodeId);
        }
    }


    // --------------------------------------------------------
    // STEP 7:
    // Execute affected nodes level by level.
    // --------------------------------------------------------

    while (!ready.empty())
    {
        std::vector<int> currentBatch;


        while (!ready.empty())
        {
            currentBatch.push_back(
                ready.front()
            );

            ready.pop();
        }


        struct PendingTask
        {
            int nodeId;

            std::future<Result> future;
        };


        std::vector<PendingTask>
            pendingTasks;


        // ----------------------------------------------------
        // Submit current batch
        // ----------------------------------------------------

        for (int nodeId :
             currentBatch)
        {
            const Node* node =
                graph.getNode(nodeId);


            auto computationIt =
                computations.find(nodeId);


            if (
                computationIt ==
                computations.end()
            )
            {
                throw std::runtime_error(
                    "No computation registered for node: " +
                    std::to_string(nodeId)
                );
            }


            std::vector<Result> inputs;


            // ------------------------------------------------
            // Gather inputs.
            //
            // Some dependencies may be newly computed.
            // Others may come directly from cache.
            // ------------------------------------------------

            for (int dependencyId :
                 node->getDependencies())
            {
                if (
                    !cacheManager.contains(
                        dependencyId
                    )
                )
                {
                    throw std::runtime_error(
                        "Required cached result "
                        "does not exist for node: " +
                        std::to_string(
                            dependencyId
                        )
                    );
                }


                inputs.push_back(
                    cacheManager.get(
                        dependencyId
                    )
                );
            }


            // ------------------------------------------------
            // Submit computation
            // ------------------------------------------------

            auto future =
                pool.submit(
                    [
                        computation =
                            computationIt->second,

                        inputs =
                            std::move(inputs)
                    ]()
                    {
                        return computation->
                            calculate(inputs);
                    }
                );


            pendingTasks.push_back(
                {
                    nodeId,
                    std::move(future)
                }
            );
        }


        // ----------------------------------------------------
        // Collect newly computed results
        // ----------------------------------------------------

        for (auto& task :
             pendingTasks)
        {
            Result result =
                task.future.get();


            cacheManager.store(
                task.nodeId,
                result
            );


            metrics.recordComputed();
        }


        // ----------------------------------------------------
        // Unlock affected dependents
        // ----------------------------------------------------

        for (
            int completedNodeId :
            currentBatch
        )
        {
            const Node* node =
                graph.getNode(
                    completedNodeId
                );


            for (
                int dependentId :
                node->getDependents()
            )
            {
                if (
                    !affectedNodes.contains(
                        dependentId
                    )
                )
                {
                    continue;
                }


                --remainingDependencies[
                    dependentId
                ];


                if (
                    remainingDependencies[
                        dependentId
                    ] == 0
                )
                {
                    ready.push(
                        dependentId
                    );
                }
            }
        }
    }


    // --------------------------------------------------------
    // STEP 8:
    // Return the complete current result set.
    // --------------------------------------------------------

    std::unordered_map<int, Result>
        results;


    for (const auto& [nodeId, node] :
         graph.getNodes())
    {
        if (
            cacheManager.contains(
                nodeId
            )
        )
        {
            results[nodeId] =
                cacheManager.get(
                    nodeId
                );
        }
    }


    return results;
}


// ============================================================
// Metrics accessor
// ============================================================

const ExecutionMetrics&
Scheduler::getMetrics() const
{
    return metrics;
}