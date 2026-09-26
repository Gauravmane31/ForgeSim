#pragma once

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
    int id;
    std::string name;
    std::vector<int> dependencies;
    std::vector<int> dependents;
    NodeState state;

public:
    Node(int id, std::string name);
    int getId() const;
    const std::string &getName() const; // used to skip making copy of itself for returning
    NodeState getState() const;
    const std::vector<int> &getDependencies() const;//we added these functions to get access of dependancies and dependants by graph  
    const std::vector<int> &getDependents() const;
    void addDependency(int id);
    void addDependent(int id);
    void setState(NodeState st);
};