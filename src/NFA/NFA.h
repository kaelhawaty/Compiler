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

// Non-deterministic automata class for string matching.
class NFA {

public:
    /**
     * Constructs an NFA that accepts any string which is simply two
     * nodes connected by EPSILON edge.
     */
    NFA();

    /**
     * Constructs an NFA that accepts a string consisting of a single char.
     */
    explicit NFA(char c);

    class Node;

    /**
     * Constructs an NFA given the start and ending pointers of the NFA.
     * Note that the end state is considered to be the accepting state.
     */
    explicit NFA(std::shared_ptr<Node> start, std::shared_ptr<Node> end);

    NFA(const NFA &cpy);

    NFA &operator=(const NFA &cpy);

    NFA(NFA &&rhs) noexcept;

    NFA &operator=(NFA &&rhs) noexcept;

    /**
     * Simple set of Nodes/States of the NFA which we define operations such as
     * Move, E_closure on, see below.
     */
    using Set = std::set<const NFA::Node *>;

    class Node {
    public:
        using Transitions = std::unordered_map<char, std::vector<std::shared_ptr<Node>>>;
        static int UNIQUE_ID;

        Node();

        int get_id() const;

        void addTransition(char c, std::shared_ptr<Node> ptr);

        friend class NFA;

        friend class NFA_Builder;

        friend NFA::Set E_closure(const NFA::Set &states);

        friend NFA::Set Move(const NFA::Set &states, const char c);

    private:
        const int id;
        Transitions trans;
    };

    friend class NFA_Builder;

    const Node *get_start() const {
        return start.get();
    }

    const Node *get_end() const {
        return end.get();
    }

private:
    // Start and end states of the automata respectively. Note that
    // the end state is considered to be the accepting state,
    // as any NFA has a single accepting state.
    std::shared_ptr<Node> start, end;

};

/**
 * Given a set of Nodes/States of the NFA, It returns ε-closure of that set
 * i.e adds to the set all the states that are connected to this set of states
 * with ε edges.
 */
NFA::Set E_closure(const NFA::Set &states);

/**
 * Given a set of Nodes/States of the NFA, it moves every state in this set
 * according to the transition character c. Note that this function doesn't call
 * ε-closure after moving the state. If needed, follow it by a call to ε-closure.
 */
NFA::Set Move(const NFA::Set &states, char c);

#endif //COMPILER_NFA_H
