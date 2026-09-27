#pragma once

#include <cstddef>
#include <future>
#include <memory>
#include <unordered_map>
#include <vector>

#include "ForgeSim/core/Computation.hpp"
#include "ForgeSim/core/Graph.hpp"
#include "ForgeSim/core/ImpactAnalyzer.hpp"
#include "ForgeSim/core/Result.hpp"
#include "ForgeSim/execution/CacheManager.hpp"
#include "ForgeSim/execution/ThreadPool.hpp"
#include "ForgeSim/execution/ExecutionMetrics.hpp"

class Scheduler
{
private:
    std::unordered_map<
        int,
        std::shared_ptr<Computation>>
        computations;

    CacheManager cacheManager;

    ImpactAnalyzer impactAnalyzer;
    ExecutionMetrics metrics;

public:
    void registerComputation(
        int nodeId,
        std::shared_ptr<Computation> computation);

    std::unordered_map<int, Result> execute(
        const Graph &graph) const;

    std::unordered_map<int, Result> executeParallel(
        const Graph &graph,
        std::size_t workerCount) const;

    /*
        Incremental execution.

        changedNodes contains the nodes whose
        input/value has changed.
    */
    std::unordered_map<int, Result> executeIncremental(
        Graph &graph,
        std::size_t workerCount);

    const ExecutionMetrics &getMetrics() const;
};