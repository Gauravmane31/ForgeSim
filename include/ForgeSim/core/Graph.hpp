#pragma once

#include <unordered_map>
#include <vector>

#include "ForgeSim/core/Node.hpp"

class Graph
{
private:
    std::unordered_map<int, Node> nodes;

public:
    bool addNode(const Node &node);

    Node *getNode(int id);
    const Node *getNode(int id) const;

    bool addDependency(int nodeId, int dependencyId);

    bool contains(int id) const;

    std::size_t getNodeCount() const;

    const std::unordered_map<int, Node> &getNodes() const;

    bool hasCycle() const;

    std::vector<int> topologicalSort() const;

    bool markChanged(int nodeId);

    std::vector<int> getDirtyNodes() const;
};