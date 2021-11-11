//
// Created by Karim and Hazem on 4/29/2021.
//

#include "NFA_Builder.h"

NFA_Builder::NFA_Builder() {

}

NFA_Builder::NFA_Builder(const NFA &nfa) : nfa(nfa) {

}

NFA_Builder &NFA_Builder::Concatenate(NFA rhs) {
    nfa.end->addTransition(EPSILON, rhs.start);
    nfa.end = rhs.end;
    nfa.nodes.insert(nfa.nodes.end(), std::make_move_iterator(rhs.nodes.begin()),
              std::make_move_iterator(rhs.nodes.end()));
    return *this;
}

NFA_Builder &NFA_Builder::Concatenate(const char c) {
    return Concatenate(NFA{c});
}

NFA_Builder &NFA_Builder::Or(NFA rhs) {
    nfa.nodes.push_back(std::make_unique<NFA::Node>());
    auto new_start = nfa.nodes.back().get();
    nfa.nodes.push_back(std::make_unique<NFA::Node>());
    auto new_end = nfa.nodes.back().get();

    new_start->addTransition(EPSILON, nfa.start);
    new_start->addTransition(EPSILON, rhs.start);
    nfa.end->addTransition(EPSILON, new_end);
    rhs.end->addTransition(EPSILON, new_end);
    nfa.start = new_start;
    nfa.end = new_end;
    nfa.nodes.insert(nfa.nodes.end(), std::make_move_iterator(rhs.nodes.begin()),
                     std::make_move_iterator(rhs.nodes.end()));
    return *this;
}

NFA_Builder &NFA_Builder::Or(const char c) {
    return Or(NFA{c});
}

NFA_Builder &NFA_Builder::Positive_closure() {
    nfa.nodes.push_back(std::make_unique<NFA::Node>());
    auto new_start = nfa.nodes.back().get();
    nfa.nodes.push_back(std::make_unique<NFA::Node>());
    auto new_end = nfa.nodes.back().get();

    nfa.end->addTransition(EPSILON, nfa.start);
    new_start->addTransition(EPSILON, nfa.start);
    nfa.end->addTransition(EPSILON, new_end);
    nfa.start = new_start;
    nfa.end = new_end;
    return *this;
}

NFA_Builder &NFA_Builder::Kleene_closure() {
    this->Positive_closure();
    nfa.start->addTransition(EPSILON, nfa.end);
    return *this;
}

/**
 * Note after calling this function, the builder object becomes unusable.
 * Since the NFA is moved out of the builder class.
 */
NFA NFA_Builder::build() {
    return std::move(nfa);
}