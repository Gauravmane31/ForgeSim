#include "ForgeSim/core/ImpactAnalyzer.hpp"

#include <queue>
#include <stdexcept>


std::unordered_set<int>
ImpactAnalyzer::findAffectedNodes(
    const Graph& graph,
    const std::vector<int>& changedNodes
) const
{
    std::unordered_set<int> affected;

    std::queue<int> pending;


    /*
        Start with nodes that directly changed.
    */

    for (int nodeId : changedNodes)
    {
        if (!graph.contains(nodeId))
        {
            throw std::runtime_error(
                "Changed node does not exist in graph: " +
                std::to_string(nodeId)
            );
        }

        pending.push(nodeId);
        affected.insert(nodeId);
    }


    /*
        Traverse the graph in the
        dependency -> dependent direction.
    */

    while (!pending.empty())
    {
        int current =
            pending.front();

        pending.pop();


        const Node* node =
            graph.getNode(current);


        for (
            int dependentId :
            node->getDependents()
        )
        {
            /*
                Only process a node the first
                time we encounter it.
            */

            if (
                affected.insert(
                    dependentId
                ).second
            )
            {
                pending.push(
                    dependentId
                );
            }
        }
    }


    return affected;
}