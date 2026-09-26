#pragma once

#include <unordered_set>
#include <vector>

#include "ForgeSim/core/Graph.hpp"


class ImpactAnalyzer
{
public:

    /*
        Given one or more changed nodes,
        find every node affected by those changes.
    */
    std::unordered_set<int> findAffectedNodes(
        const Graph& graph,
        const std::vector<int>& changedNodes
    ) const;
};