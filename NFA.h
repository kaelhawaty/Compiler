//
// Created by Karim and Hazem on 4/28/2021.
//

#ifndef COMPILER_NFA_H
#define COMPILER_NFA_H

#include <memory>
#include <vector>
#include <unordered_map>
#include <queue>
#include <unordered_set>
#include <set>

const char EPSILON = 0;

class NFA {
public:
    NFA(const char c);

    NFA(const NFA &cpy);

    NFA &operator=(const NFA &cpy);

    NFA(NFA &&rhs);

    NFA &operator=(NFA &&rhs);

    class Node;

    using Set = std::set<const NFA::Node *>;
private:
    std::shared_ptr<Node> start, end;
};

NFA Concatenate(const NFA &lhs, const NFA &rhs);

NFA Or(const NFA &lhs, const NFA &rhs);

NFA Kleene_closure(const NFA &nfa);

NFA Positive_closure(const NFA &nfa);

const NFA::Set E_closure(const NFA::Set &states);

const NFA::Set Move(const NFA::Set &states, const char c);

#endif //COMPILER_NFA_H
