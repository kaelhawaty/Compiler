//
// Created by Karim and Hazem on 4/28/2021.
//

#include "NFA.h"


int NFA::Node::UNIQUE_ID = 0;

NFA::Node::Node() : id(UNIQUE_ID++) {}

int NFA::Node::get_id() const {
    return id;
}

void NFA::Node::addTransition(const char c, Node *ptr) {
    this->trans[c].push_back(ptr);
}

NFA::NFA() {
    nodes.emplace_back(std::make_unique<Node>());
    nodes.emplace_back(std::make_unique<Node>());
    start = nodes[0].get();
    end = nodes[1].get();
    start->addTransition(EPSILON, end);
}

NFA::NFA(const char c) {
    nodes.emplace_back(std::make_unique<Node>());
    nodes.emplace_back(std::make_unique<Node>());
    start = nodes[0].get();
    end = nodes[1].get();
    start->addTransition(c, end);
}

NFA::NFA(NFA::Node *start, NFA::Node *end, std::vector<std::unique_ptr<Node>> nodes) : start(start), end(end),
                                                                                       nodes(std::move(nodes)) {}

/*
 * Copy constructor in O(n) complexity by traversing the NFA and copying each node/state in it.
 */
NFA::NFA(const NFA &cpy) {
    // Check if we have allocated this node or not.
    std::unordered_map<int, std::unique_ptr<Node>> allocated;
    // Check if the node has been constructed. By construction we mean adding
    // its transitions using the old graph.
    std::unordered_set<int> constructed{cpy.start->id};
    // Traverse NFA to copy nodes.
    std::queue<Node *> q;
    q.push(cpy.start);
    while (!q.empty()) {
        Node *cur_old = q.front();
        q.pop();

        if (auto it = allocated.find(cur_old->id); it == allocated.end()) {
            allocated[cur_old->id] = std::make_unique<Node>();
        }

        Node *cur_new = allocated[cur_old->id].get();
        for (const auto&[c, vec] : cur_old->trans) {
            for (const auto &node_ptr: vec) {

                if (auto it = allocated.find(node_ptr->id); it == allocated.end()) {
                    allocated[node_ptr->id] = std::make_unique<Node>();
                }

                cur_new->trans[c].push_back(allocated[node_ptr->id].get());
                if (!constructed.count(node_ptr->id)) {
                    constructed.insert(node_ptr->id);
                    q.push(node_ptr);
                }
            }
        }
    }
    this->start = allocated[cpy.start->id].get();
    this->end = allocated[cpy.end->id].get();

    this->nodes.reserve(allocated.size());
    for (auto &ent : allocated) {
        this->nodes.emplace_back(std::move(ent.second));
    }
}

NFA::NFA(NFA &&rhs) noexcept {
    this->start = rhs.start;
    this->end = rhs.end;
    this->nodes = std::move(rhs.nodes);
}

NFA &NFA::operator=(const NFA &cpy) {
    NFA temp(cpy);
    this->start = temp.start;
    this->end = temp.end;
    this->nodes = std::move(temp.nodes);
    return *this;
}

NFA &NFA::operator=(NFA &&rhs) noexcept {
    this->start = rhs.start;
    this->end = rhs.end;
    this->nodes = std::move(rhs.nodes);
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
        auto it = front->trans.find(EPSILON);
        if (it == front->trans.end()) {
            continue;
        }
        for (const auto &child : it->second) {
            if (closure.insert(child).second) {
                q.push(child);
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
        auto it = state->trans.find(c);
        if (it == state->trans.end()) {
            continue;
        }
        for (const auto &child : it->second) {
            new_set.insert(child);
        }
    }
    return new_set;
}
