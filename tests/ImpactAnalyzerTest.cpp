#include <gtest/gtest.h>

#include "ForgeSim/core/ImpactAnalyzer.hpp"

TEST(ImpactAnalyzerTest, FindsAllDownstreamNodes)
{
    Graph graph;

    graph.addNode(Node(1, "A"));
    graph.addNode(Node(2, "B"));
    graph.addNode(Node(3, "C"));
    graph.addNode(Node(4, "D"));

    graph.addDependency(2, 1);
    graph.addDependency(3, 2);
    graph.addDependency(4, 3);

    ImpactAnalyzer analyzer;

    const auto affected =
        analyzer.findAffectedNodes(
            graph,
            {1}
        );

    EXPECT_EQ(affected.size(), 4);

    EXPECT_TRUE(affected.contains(1));
    EXPECT_TRUE(affected.contains(2));
    EXPECT_TRUE(affected.contains(3));
    EXPECT_TRUE(affected.contains(4));
}

TEST(ImpactAnalyzerTest, DoesNotIncludeUnrelatedBranch)
{
    Graph graph;

    graph.addNode(Node(1, "A"));
    graph.addNode(Node(2, "B"));
    graph.addNode(Node(3, "C"));
    graph.addNode(Node(4, "D"));

    graph.addDependency(2, 1);

    graph.addDependency(4, 3);

    ImpactAnalyzer analyzer;

    const auto affected =
        analyzer.findAffectedNodes(
            graph,
            {1}
        );

    EXPECT_TRUE(affected.contains(1));
    EXPECT_TRUE(affected.contains(2));

    EXPECT_FALSE(affected.contains(3));
    EXPECT_FALSE(affected.contains(4));
}

TEST(ImpactAnalyzerTest, HandlesDiamondDependency)
{
    /*
                B
               / \
        A ────     ──── D
               \ /
                C
    */

    Graph graph;

    graph.addNode(Node(1, "A"));
    graph.addNode(Node(2, "B"));
    graph.addNode(Node(3, "C"));
    graph.addNode(Node(4, "D"));

    graph.addDependency(2, 1);
    graph.addDependency(3, 1);
    graph.addDependency(4, 2);
    graph.addDependency(4, 3);

    ImpactAnalyzer analyzer;

    const auto affected =
        analyzer.findAffectedNodes(
            graph,
            {1}
        );

    EXPECT_EQ(affected.size(), 4);

    EXPECT_TRUE(affected.contains(1));
    EXPECT_TRUE(affected.contains(2));
    EXPECT_TRUE(affected.contains(3));
    EXPECT_TRUE(affected.contains(4));
}
