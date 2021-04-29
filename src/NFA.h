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
    NFA();

    explicit NFA(char c);

    class Node;

    explicit NFA(std::shared_ptr<Node> start, std::shared_ptr<Node> end);

    NFA(const NFA &cpy);

    NFA &operator=(const NFA &cpy);

    NFA(NFA &&rhs) noexcept;

    NFA &operator=(NFA &&rhs) noexcept;


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
    std::shared_ptr<Node> start, end;

};

NFA::Set E_closure(const NFA::Set &states);

NFA::Set Move(const NFA::Set &states, char c);

#endif //COMPILER_NFA_H
