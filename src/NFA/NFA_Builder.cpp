//
// Created by Karim and Hazem on 4/29/2021.
//

#include "NFA_Builder.h"

NFA_Builder::NFA_Builder() {

}

NFA_Builder::NFA_Builder(const NFA &nfa) : nfa(nfa) {

}

NFA_Builder &NFA_Builder::Concatenate(NFA rhs) {
    nfa.end->addTransition(EPSILON, std::move(rhs.start));
    nfa.end = std::move(rhs.end);
    return *this;
}

NFA_Builder &NFA_Builder::Concatenate(const char c) {
    return Concatenate(NFA{c});
}

NFA_Builder &NFA_Builder::Or(NFA rhs) {
    auto new_start{std::make_shared<NFA::Node>()};
    auto new_end{std::make_shared<NFA::Node>()};

    new_start->addTransition(EPSILON, std::move(nfa.start));
    new_start->addTransition(EPSILON, std::move(rhs.start));
    nfa.end->addTransition(EPSILON, new_end);
    rhs.end->addTransition(EPSILON, new_end);
    nfa.start = std::move(new_start);
    nfa.end = std::move(new_end);
    return *this;
}

NFA_Builder &NFA_Builder::Or(const char c) {
    return Or(NFA{c});
}

NFA_Builder &NFA_Builder::Positive_closure() {
    auto new_start{std::make_shared<NFA::Node>()};
    auto new_end{std::make_shared<NFA::Node>()};

    nfa.end->addTransition(EPSILON, nfa.start);
    new_start->addTransition(EPSILON, std::move(nfa.start));
    nfa.end->addTransition(EPSILON, new_end);
    nfa.start = std::move(new_start);
    nfa.end = std::move(new_end);
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