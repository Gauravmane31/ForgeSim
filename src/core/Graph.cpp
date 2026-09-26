#include "ForgeSim/core/Graph.hpp"

#include <queue>
#include <stdexcept>

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

bool Graph::hasCycle() const {

    std::unordered_map<int, int> inDegree;

    for (const auto& [id, node] : nodes) {
        inDegree[id] = 0;
    }

    for (const auto& [id, node] : nodes) {

        const auto& dependents = node.getDependents();

        for (int dependentId : dependents) {
            ++inDegree[dependentId];
        }
    }

    std::queue<int> ready;

    for (const auto& [id, degree] : inDegree) {
        if (degree == 0) {
            ready.push(id);
        }
    }

    std::size_t processed = 0;

    while (!ready.empty()) {

        int current = ready.front();
        ready.pop();

        ++processed;

        const Node* node = getNode(current);

        for (int dependentId : node->getDependents()) {

            --inDegree[dependentId];

            if (inDegree[dependentId] == 0) {
                ready.push(dependentId);
            }
        }
    }

    return processed != nodes.size();
}

std::vector<int> Graph::topologicalSort() const {

    std::unordered_map<int, int> inDegree;

    for (const auto& [id, node] : nodes) {
        inDegree[id] = 0;
    }

    for (const auto& [id, node] : nodes) {

        for (int dependentId : node.getDependents()) {
            ++inDegree[dependentId];
        }
    }

    std::queue<int> ready;

    for (const auto& [id, degree] : inDegree) {
        if (degree == 0) {
            ready.push(id);
        }
    }

    std::vector<int> order;
    order.reserve(nodes.size());

    while (!ready.empty()) {

        int current = ready.front();
        ready.pop();

        order.push_back(current);

        const Node* node = getNode(current);

        for (int dependentId : node->getDependents()) {

            --inDegree[dependentId];

            if (inDegree[dependentId] == 0) {
                ready.push(dependentId);
            }
        }
    }

    if (order.size() != nodes.size()) {
        throw std::runtime_error(
            "Graph contains a cycle. Topological ordering is impossible."
        );
    }

    return order;
}