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
    explicit NFA(char c);

    NFA(const NFA &cpy);

    NFA &operator=(const NFA &cpy);

    NFA(NFA &&rhs) noexcept;

    NFA &operator=(NFA &&rhs) noexcept;

    class Node;

    using Set = std::set<const NFA::Node *>;
    std::shared_ptr<Node> start, end;

};

NFA Concatenate(NFA lhs, NFA rhs);

NFA Or(NFA lhs, NFA rhs);

NFA Kleene_closure(NFA nfa);

NFA Positive_closure(NFA nfa);

NFA::Set E_closure(const NFA::Set &states);

NFA::Set Move(const NFA::Set &states, char c);

#endif //COMPILER_NFA_H
