#include "ForgeSim/core/Node.hpp"

#include <algorithm>
#include <utility>

Node::Node(int id, std::string name)
    : id(id),
      name(std::move(name)),
      state(NodeState::DIRTY),
      version(0)
{
}

int Node::getId() const
{
    return id;
}

const std::string& Node::getName() const
{
    return name;
}

NodeState Node::getState() const
{
    return state;
}

const std::vector<int>& Node::getDependencies() const
{
    return dependencies;
}

const std::vector<int>& Node::getDependents() const
{
    return dependents;
}

std::size_t Node::getVersion() const
{
    return version;
}

void Node::addDependency(int dependencyId)
{
    if (std::find(
            dependencies.begin(),
            dependencies.end(),
            dependencyId
        ) == dependencies.end())
    {
        dependencies.push_back(dependencyId);
    }
}

void Node::addDependent(int dependentId)
{
    if (std::find(
            dependents.begin(),
            dependents.end(),
            dependentId
        ) == dependents.end())
    {
        dependents.push_back(dependentId);
    }
}

void Node::setState(NodeState newState)
{
    state = newState;
}

void Node::markDirty()
{
    ++version;
    state = NodeState::DIRTY;
}