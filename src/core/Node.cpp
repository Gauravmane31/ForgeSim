#include "ForgeSim/core/Node.hpp"

Node::Node(int id, std::string name)
    : id(id), name(name), state(NodeState::DIRTY) {
}

int Node::getId() const {
    return id;
}

const std::string& Node::getName() const {
    return name;
}

NodeState Node::getState() const {
    return state;
}

void Node::addDependency(int id){
    dependencies.push_back(id); 
}
void Node::addDependent(int id){
    dependents.push_back(id);
}

void Node::setState(NodeState st){
    state=st;
}

const std::vector<int>& Node::getDependencies() const {
    return dependencies;
}

const std::vector<int>& Node::getDependents() const {
    return dependents;
}