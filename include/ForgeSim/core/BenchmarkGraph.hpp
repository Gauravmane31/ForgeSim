#pragma once

#include <cstddef>
#include "ForgeSim/core/Graph.hpp"

class BenchmarkGraph
{
public:
    static Graph create(
        std::size_t nodeCount,
        std::size_t branchFactor
    );
};