#pragma once

#include <cstddef>
#include <string>
#include <vector>


enum class NodeState
{
    DIRTY,
    READY,
    RUNNING,
    COMPLETED,
    FAILED
};


class Node
{
private:

    int id;

    std::string name;

    std::vector<int> dependencies;

    std::vector<int> dependents;

    NodeState state;

    std::size_t version;


public:

    Node(
        int id,
        std::string name
    );


    int getId() const;

    const std::string& getName() const;

    NodeState getState() const;

    const std::vector<int>&
    getDependencies() const;

    const std::vector<int>&
    getDependents() const;


    std::size_t getVersion() const;


    void addDependency(
        int id
    );

    void addDependent(
        int id
    );


    void setState(
        NodeState st
    );


    /*
        Mark this node as changed.

        The version is incremented so that
        the engine can distinguish the new
        state from the previous execution.
    */
    void markDirty();
};