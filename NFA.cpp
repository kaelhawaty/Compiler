//
// Created by Karim and Hazem on 4/28/2021.
//

#include "NFA.h"

class NFA::Node {

public:
    using Transitions = std::unordered_map<char, std::vector<std::shared_ptr<Node>>>;
    static int UNIQUE_ID;

    Node() : id(UNIQUE_ID++) {}

    int get_id() const {
        return id;
    }

    friend class NFA;

    friend NFA Concatenate(NFA lhs, NFA rhs);

    friend NFA Or(NFA lhs, NFA rhs);

    friend NFA Kleene_closure(NFA nfa);

    friend NFA Positive_closure(NFA nfa);

    friend NFA::Set E_closure(const NFA::Set &states);

    friend NFA::Set Move(const NFA::Set &states, const char c);

private:
    const int id;
    Transitions trans;
};

int NFA::Node::UNIQUE_ID = 0;


NFA::NFA(const char c) : start(std::make_shared<Node>()), end(std::make_shared<Node>()) {
    start->trans[c].push_back(end);
}

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
                    constructed.insert(cur_old->id);
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

NFA Concatenate(NFA lhs, NFA rhs) {
    lhs.end->trans[EPSILON].push_back(std::move(rhs.start));
    lhs.end = std::move(rhs.end);
    return lhs;
}

NFA Or(NFA lhs, NFA rhs) {
    auto new_start{std::make_shared<NFA::Node>()};
    auto new_end{std::make_shared<NFA::Node>()};

    new_start->trans[EPSILON].push_back(std::move(lhs.start));
    new_start->trans[EPSILON].push_back(std::move(rhs.start));
    lhs.end->trans[EPSILON].push_back(new_end);
    rhs.end->trans[EPSILON].push_back(new_end);
    lhs.start = std::move(new_start);
    lhs.end = std::move(new_end);
    return lhs;
}

NFA Kleene_closure(NFA nfa) {
    NFA new_nfa{Positive_closure(nfa)};
    new_nfa.start->trans[EPSILON].push_back(new_nfa.end);
    return nfa;
}

NFA Positive_closure(NFA nfa) {
    auto new_start{std::make_shared<NFA::Node>()};
    auto new_end{std::make_shared<NFA::Node>()};

    nfa.end->trans[EPSILON].push_back(std::move(nfa.start));
    new_start->trans[EPSILON].push_back(std::move(nfa.start));
    nfa.end->trans[EPSILON].push_back(new_end);
    nfa.start = std::move(new_start);
    nfa.end = std::move(new_end);
    return nfa;
}

NFA::Set E_closure(const NFA::Set &states) {
    NFA::Set closure = states;
    std::queue<const NFA::Node *> q;
    for (auto &state : states) {
        q.push(state);
    }
    while (!q.empty()) {
        auto front = q.front();
        q.pop();
        for (const auto &child : front->trans.at(EPSILON)) {
            if (!closure.count(child.get())) {
                closure.insert(child.get());
                q.push(child.get());
            }
        }
    }
    return closure;
}

NFA::Set Move(const NFA::Set &states, const char c) {
    NFA::Set new_set;
    for (auto &state : states) {
        for (const auto &child : state->trans.at(c)) {
            new_set.insert(child.get());
        }
    }
    return E_closure(new_set);
}







