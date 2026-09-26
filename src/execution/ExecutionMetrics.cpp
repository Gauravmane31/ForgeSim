#include "ForgeSim/execution/ExecutionMetrics.hpp"


void ExecutionMetrics::reset()
{
    totalNodes = 0;
    computedNodes = 0;
    cachedNodes = 0;
}


void ExecutionMetrics::setTotalNodes(
    std::size_t count
)
{
    totalNodes = count;
}


void ExecutionMetrics::recordComputed()
{
    ++computedNodes;
}


void ExecutionMetrics::recordCacheHit()
{
    ++cachedNodes;
}


std::size_t ExecutionMetrics::getTotalNodes() const
{
    return totalNodes;
}


std::size_t ExecutionMetrics::getComputedNodes() const
{
    return computedNodes;
}


std::size_t ExecutionMetrics::getCachedNodes() const
{
    return cachedNodes;
}


double ExecutionMetrics::getCacheHitRate() const
{
    if (totalNodes == 0)
    {
        return 0.0;
    }

    return (
        static_cast<double>(cachedNodes)
        /
        static_cast<double>(totalNodes)
    ) * 100.0;
}