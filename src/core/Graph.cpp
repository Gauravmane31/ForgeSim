#include "ForgeSim/core/Graph.hpp"

bool Graph::addNode(const Node& node) {
    auto [it, inserted] = nodes.emplace(node.getId(), node);
    return inserted;
}

Node* Graph::getNode(int id) {
    auto it = nodes.find(id);

    if (it == nodes.end()) {
        return nullptr;
    }

    return &it->second;
}

const Node* Graph::getNode(int id) const {
    auto it = nodes.find(id);

    if (it == nodes.end()) {
        return nullptr;
    }

    return &it->second;
}

bool Graph::addDependency(int nodeId, int dependencyId) {

    Node* node = getNode(nodeId);
    Node* dependency = getNode(dependencyId);

    if (node == nullptr || dependency == nullptr) {
        return false;
    }

    node->addDependency(dependencyId);
    dependency->addDependent(nodeId);

    return true;
}

bool Graph::contains(int id) const {
    return nodes.find(id) != nodes.end();
}

std::size_t Graph::getNodeCount() const {
    return nodes.size();
}

const std::unordered_map<int, Node>& Graph::getNodes() const {
    return nodes;
}