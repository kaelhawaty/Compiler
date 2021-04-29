//
// Created by Karim and Hazem on 4/28/2021.
//

#include "NFA.h"


int NFA::Node::UNIQUE_ID = 0;

NFA::Node::Node() : id(UNIQUE_ID++) {}

int NFA::Node::get_id() const {
    return id;
}

void NFA::Node::addTransition(const char c, std::shared_ptr<Node> ptr) {
    this->trans[c].push_back(std::move(ptr));
}

NFA::NFA() : start(std::make_shared<Node>()), end(std::make_shared<Node>()) {
    start->addTransition(EPSILON, end);
}

NFA::NFA(const char c) : start(std::make_shared<Node>()), end(std::make_shared<Node>()) {
    start->addTransition(c, end);
}

NFA::NFA(std::shared_ptr<Node> start, std::shared_ptr<Node> end) : start(std::move(start)), end(std::move(end)) {

}

/*
 * Copy constructor in O(n) complexity by traversing the NFA and copying each node/state in it.
 */
NFA::NFA(const NFA &cpy) {
    // Check if we have allocated this node or not.
    std::unordered_map<int, std::shared_ptr<Node>> allocated;
    // Check if the node has been constructed. By construction we mean adding
    // its transitions using the old graph.
    std::unordered_set<int> constructed{cpy.start->id};
    // Traverse NFA to copy nodes.
    std::queue<Node *> q;
    q.push(cpy.start.get());
    while (!q.empty()) {
        Node *cur_old = q.front();
        q.pop();

        if (auto it = allocated.find(cur_old->id); it == allocated.end()) {
            allocated[cur_old->id] = std::make_shared<Node>();
        }

        Node *cur_new = allocated[cur_old->id].get();
        for (const auto&[c, vec] : cur_old->trans) {
            for (const auto &node_ptr: vec) {

                if (auto it = allocated.find(node_ptr->id); it == allocated.end()) {
                    allocated[node_ptr->id] = std::make_shared<Node>();
                }

                cur_new->trans[c].push_back(allocated[node_ptr->id]);
                if (!constructed.count(node_ptr->id)) {
                    constructed.insert(node_ptr->id);
                    q.push(node_ptr.get());
                }
            }
        }
    }
    this->start = std::move(allocated[cpy.start->id]);
    this->end = std::move(allocated[cpy.end->id]);
}

NFA::NFA(NFA &&rhs) noexcept {
    this->start = std::move(rhs.start);
    this->end = std::move(rhs.end);
}

NFA &NFA::operator=(const NFA &cpy) {
    NFA temp(cpy);
    this->start = std::move(temp.start);
    this->end = std::move(temp.end);
    return *this;
}

NFA &NFA::operator=(NFA &&rhs) noexcept {
    this->start = std::move(rhs.start);
    this->end = std::move(rhs.end);
    return *this;
}

/*
 * Compute the ε-closure of the given set by using a bfs that traverses the
 * edges connected to the current set using ε edges/transitions.
 */
NFA::Set E_closure(const NFA::Set &states) {
    NFA::Set closure = states;
    std::queue<const NFA::Node *> q;
    for (auto &state : states) {
        q.push(state);
    }
    while (!q.empty()) {
        auto front = q.front();
        q.pop();
        if (!front->trans.count(EPSILON)) {
            continue;
        }
        for (const auto &child : front->trans.at(EPSILON)) {
            if (!closure.count(child.get())) {
                closure.insert(child.get());
                q.push(child.get());
            }
        }
    }
    return closure;
}

/*
 * Moves each state in the set using the c transition provided in the argument.
 * Note that this function doesn't call ε-closure and if needed, follow it by a call to
 * ε-closure.
 */
NFA::Set Move(const NFA::Set &states, const char c) {
    NFA::Set new_set;
    for (auto &state : states) {
        if (!state->trans.count(c)) {
            continue;
        }
        for (const auto &child : state->trans.at(c)) {
            new_set.insert(child.get());
        }
    }
    return new_set;
}