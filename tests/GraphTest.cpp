#include <gtest/gtest.h>

#include "ForgeSim/core/Graph.hpp"

TEST(GraphTest, NodesCanBeAdded)
{
    Graph graph;

    EXPECT_TRUE(
        graph.addNode(
            Node(1, "Engine RPM")
        )
    );

    EXPECT_TRUE(
        graph.addNode(
            Node(2, "Gear Ratio")
        )
    );

    EXPECT_EQ(graph.getNodeCount(), 2);
    EXPECT_TRUE(graph.contains(1));
    EXPECT_TRUE(graph.contains(2));
}

TEST(GraphTest, DuplicateNodeIsRejected)
{
    Graph graph;

    EXPECT_TRUE(
        graph.addNode(
            Node(1, "Engine RPM")
        )
    );

    EXPECT_FALSE(
        graph.addNode(
            Node(1, "Duplicate")
        )
    );

    EXPECT_EQ(graph.getNodeCount(), 1);
}

TEST(GraphTest, DependenciesAreCreatedCorrectly)
{
    Graph graph;

    graph.addNode(Node(1, "Engine RPM"));
    graph.addNode(Node(2, "Gear Ratio"));
    graph.addNode(Node(3, "Output RPM"));

    EXPECT_TRUE(
        graph.addDependency(3, 1)
    );

    EXPECT_TRUE(
        graph.addDependency(3, 2)
    );

    const Node* output =
        graph.getNode(3);

    ASSERT_NE(output, nullptr);

    EXPECT_EQ(
        output->getDependencies().size(),
        2
    );

    const Node* rpm =
        graph.getNode(1);

    ASSERT_NE(rpm, nullptr);

    EXPECT_EQ(
        rpm->getDependents().size(),
        1
    );
}

TEST(GraphTest, AcyclicGraphHasNoCycle)
{
    Graph graph;

    graph.addNode(Node(1, "A"));
    graph.addNode(Node(2, "B"));
    graph.addNode(Node(3, "C"));

    graph.addDependency(2, 1);
    graph.addDependency(3, 2);

    EXPECT_FALSE(graph.hasCycle());
}

TEST(GraphTest, CyclicGraphIsDetected)
{
    Graph graph;

    graph.addNode(Node(1, "A"));
    graph.addNode(Node(2, "B"));
    graph.addNode(Node(3, "C"));

    graph.addDependency(2, 1);
    graph.addDependency(3, 2);
    graph.addDependency(1, 3);

    EXPECT_TRUE(graph.hasCycle());
}

TEST(GraphTest, TopologicalSortProducesValidOrder)
{
    Graph graph;

    graph.addNode(Node(1, "A"));
    graph.addNode(Node(2, "B"));
    graph.addNode(Node(3, "C"));

    graph.addDependency(2, 1);
    graph.addDependency(3, 2);

    const auto order =
        graph.topologicalSort();

    ASSERT_EQ(order.size(), 3);

    auto positionOf =
        [&order](int id)
    {
        for (std::size_t i = 0;
             i < order.size();
             ++i)
        {
            if (order[i] == id)
            {
                return i;
            }
        }

        return order.size();
    };

    EXPECT_LT(
        positionOf(1),
        positionOf(2)
    );

    EXPECT_LT(
        positionOf(2),
        positionOf(3)
    );
}

TEST(GraphTest, DirtyNodesCanBeDetected)
{
    Graph graph;

    graph.addNode(Node(1, "A"));
    graph.addNode(Node(2, "B"));
    graph.addNode(Node(3, "C"));

    graph.getNode(1)->setState(
        NodeState::COMPLETED
    );

    graph.getNode(2)->setState(
        NodeState::COMPLETED
    );

    graph.getNode(3)->setState(
        NodeState::COMPLETED
    );

    graph.markChanged(2);

    const auto dirtyNodes =
        graph.getDirtyNodes();

    ASSERT_EQ(dirtyNodes.size(), 1);
    EXPECT_EQ(dirtyNodes[0], 2);
}

TEST(GraphTest, DuplicateDependencyIsIgnored)
{
    Graph graph;

    graph.addNode(Node(1, "A"));
    graph.addNode(Node(2, "B"));

    ASSERT_TRUE(
        graph.addDependency(2, 1)
    );

    ASSERT_TRUE(
        graph.addDependency(2, 1)
    );

    const Node* node2 =
        graph.getNode(2);

    const Node* node1 =
        graph.getNode(1);

    ASSERT_NE(node2, nullptr);
    ASSERT_NE(node1, nullptr);

    EXPECT_EQ(
        node2->getDependencies().size(),
        1
    );

    EXPECT_EQ(
        node1->getDependents().size(),
        1
    );
}