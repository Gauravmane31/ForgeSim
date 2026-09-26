#include <iostream>

#include "ForgeSim/core/Graph.hpp"

int main() {

    Graph graph;

    Node rpm(1, "Engine RPM");
    Node torque(2, "Input Torque");
    Node ratio(3, "Gear Ratio");

    graph.addNode(rpm);
    graph.addNode(torque);
    graph.addNode(ratio);

    graph.addDependency(2, 1);
    graph.addDependency(3, 2);

    std::cout << "ForgeSim Graph Test\n";
    std::cout << "===================\n";

    std::cout << "Nodes: "
              << graph.getNodeCount()
              << '\n';

    std::cout << "Engine RPM exists: "
              << graph.contains(1)
              << '\n';

    std::cout << "Input Torque exists: "
              << graph.contains(2)
              << '\n';

    std::cout << "Gear Ratio exists: "
              << graph.contains(3)
              << '\n';

    return 0;
}