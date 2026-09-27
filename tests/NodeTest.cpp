#include <gtest/gtest.h>

#include "ForgeSim/core/Node.hpp"

TEST(NodeTest, ConstructorInitializesCorrectly)
{
    Node node(1, "Engine RPM");

    EXPECT_EQ(node.getId(), 1);
    EXPECT_EQ(node.getName(), "Engine RPM");
    EXPECT_EQ(node.getState(), NodeState::DIRTY);
    EXPECT_EQ(node.getVersion(), 0);
}

TEST(NodeTest, DependenciesCanBeAdded)
{
    Node node(2, "Output RPM");

    node.addDependency(1);
    node.addDependency(3);

    const auto& dependencies =
        node.getDependencies();

    EXPECT_EQ(dependencies.size(), 2);
    EXPECT_EQ(dependencies[0], 1);
    EXPECT_EQ(dependencies[1], 3);
}

TEST(NodeTest, DependentsCanBeAdded)
{
    Node node(1, "Engine RPM");

    node.addDependent(2);
    node.addDependent(3);

    const auto& dependents =
        node.getDependents();

    EXPECT_EQ(dependents.size(), 2);
    EXPECT_EQ(dependents[0], 2);
    EXPECT_EQ(dependents[1], 3);
}

TEST(NodeTest, MarkDirtyIncrementsVersion)
{
    Node node(1, "Engine RPM");

    EXPECT_EQ(node.getVersion(), 0);
    EXPECT_EQ(node.getState(), NodeState::DIRTY);

    node.setState(NodeState::COMPLETED);

    node.markDirty();

    EXPECT_EQ(node.getVersion(), 1);
    EXPECT_EQ(node.getState(), NodeState::DIRTY);

    node.markDirty();

    EXPECT_EQ(node.getVersion(), 2);
}

TEST(NodeTest, DuplicateDependenciesAreIgnored)
{
    Node node(1, "Node");

    node.addDependency(2);
    node.addDependency(2);
    node.addDependency(2);

    ASSERT_EQ(
        node.getDependencies().size(),
        1
    );

    EXPECT_EQ(
        node.getDependencies().front(),
        2
    );
}

TEST(NodeTest, DuplicateDependentsAreIgnored)
{
    Node node(1, "Node");

    node.addDependent(2);
    node.addDependent(2);
    node.addDependent(2);

    ASSERT_EQ(
        node.getDependents().size(),
        1
    );

    EXPECT_EQ(
        node.getDependents().front(),
        2
    );
}

TEST(NodeTest, InitialStateIsDirty)
{
    Node node(1, "Node");

    EXPECT_EQ(
        node.getState(),
        NodeState::DIRTY
    );
}

TEST(NodeTest, StateCanTransitionThroughExecutionLifecycle)
{
    Node node(1, "Node");

    node.setState(NodeState::READY);

    EXPECT_EQ(
        node.getState(),
        NodeState::READY
    );

    node.setState(NodeState::RUNNING);

    EXPECT_EQ(
        node.getState(),
        NodeState::RUNNING
    );

    node.setState(NodeState::COMPLETED);

    EXPECT_EQ(
        node.getState(),
        NodeState::COMPLETED
    );
}

TEST(NodeTest, MarkDirtyFromCompletedIncrementsVersion)
{
    Node node(1, "Node");

    node.setState(NodeState::COMPLETED);

    const std::size_t initialVersion =
        node.getVersion();

    node.markDirty();

    EXPECT_EQ(
        node.getState(),
        NodeState::DIRTY
    );

    EXPECT_EQ(
        node.getVersion(),
        initialVersion + 1
    );
}

TEST(NodeTest, RunningNodeCanFail)
{
    Node node(1, "Node");

    node.setState(NodeState::READY);
    node.setState(NodeState::RUNNING);
    node.setState(NodeState::FAILED);

    EXPECT_EQ(node.getState(), NodeState::FAILED);
}