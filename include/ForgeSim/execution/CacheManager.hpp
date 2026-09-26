#pragma once

#include <cstddef>
#include <unordered_map>

#include "ForgeSim/core/Result.hpp"


class CacheManager
{
private:
    std::unordered_map<int, Result> cache;

public:

    // Store or replace the result of a node.
    void store(
        int nodeId,
        const Result& result
    );

    // Check whether a valid cached result exists.
    bool contains(
        int nodeId
    ) const;

    // Retrieve a cached result.
    const Result& get(
        int nodeId
    ) const;

    // Remove one node from the cache.
    void invalidate(
        int nodeId
    );

    // Remove everything.
    void clear();

    // Number of cached results.
    std::size_t size() const;
};  