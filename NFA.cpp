//
// Created by Karim and Hazem on 4/28/2021.
//

#include "NFA.h"

class NFA::Node {
    friend class NFA;

public:
    using Transitions = std::unordered_map<char, std::vector<std::shared_ptr<Node>>>;
    static int UNIQUE_ID;

    Node() : id(UNIQUE_ID++) {}

    int get_id() const {
        return id;
    }

private:
    Transitions trans;
    const int id;
};

int NFA::Node::UNIQUE_ID = 0;

NFA::NFA(const char c) : start(std::make_shared<Node>()), end(std::make_shared<Node>()) {
    start->trans[c].push_back(end);
}

NFA::NFA(const NFA &cpy) {
    // Check if we have allocated this node or not.
    std::unordered_map<int, std::shared_ptr<Node>> seen;
    // Check if the node has been constructed. By construction we mean adding
    // its transitions using the old graph.
    std::unordered_set<int> constructed{cpy.start->id};
    // Traverse NFA to copy nodes.
    std::queue<Node *> q;
    q.push(cpy.start.get());
    while (!q.empty()) {
        Node *cur_old = q.front();
        q.pop();

        if (auto it = seen.find(cur_old->id); it == seen.end()) {
            seen[cur_old->id] = std::make_shared<Node>();
        }

        Node *cur_new = seen[cur_old->id].get();
        for (const auto&[c, vec] : cur_old->trans) {
            for (const auto &node_ptr: vec) {

                if (auto it = seen.find(node_ptr->id); it == seen.end()) {
                    seen[node_ptr->id] = std::make_shared<Node>();
                }

                cur_new->trans[c].push_back(seen[node_ptr->id]);
                if (!constructed.count(node_ptr->id)) {
                    constructed.insert(cur_old->id);
                    q.push(node_ptr.get());
                }
            }
        }
    }
}

NFA::NFA(NFA &&rhs) {
    this->start = std::move(rhs.start);
    this->end = std::move(rhs.end);
}

NFA &NFA::operator=(const NFA &cpy) {
    NFA temp(cpy);
    this->start = std::move(temp.start);
    this->end = std::move(temp.end);
    return *this;
}

NFA &NFA::operator=(NFA &&rhs) {
    this->start = std::move(rhs.start);
    this->end = std::move(rhs.end);
    return *this;
}

NFA Concatenate(const NFA &lhs, const NFA &rhs) {

}

NFA Or(const NFA &lhs, const NFA &rhs) {

}

NFA Kleene_closure(const NFA &nfa) {

}

NFA Positive_closure(const NFA &nfa) {

}

const NFA::Set E_closure(const NFA::Set &states) {

}

const NFA::Set Move(const NFA::Set &states, const char c) {

}







