#pragma once

#include <cstddef>


class ExecutionMetrics
{
private:
    std::size_t totalNodes = 0;
    std::size_t computedNodes = 0;
    std::size_t cachedNodes = 0;


public:

    void reset();

    void setTotalNodes(
        std::size_t count
    );

    void recordComputed();

    void recordCacheHit();

    std::size_t getTotalNodes() const;

    std::size_t getComputedNodes() const;

    std::size_t getCachedNodes() const;

    double getCacheHitRate() const;
};