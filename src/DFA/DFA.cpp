//
// Created by hazem and ziad and mahmoud and kareem on 5/2/2021.
//

#include "DFA.h"

// Construct DFA from list of NFAs
DFA::DFA(const std::vector<RegularExpression> &NFAs) {
    std::queue<NFA::Set> unmarked_states;// new unvisited states.
    std::map<NFA::Set, int> visited;// states that are visited before key->set of NFA states, value->DFA state index.
    NFA::Set start;// set of NFA start node of all NFAs.
    for (const auto &nfa : NFAs) {
        start.insert(nfa.getNFA().get_start());
    }
    start = E_closure(start);
    int state_id = 0;
    visited[start] = state_id;
    states.emplace_back(state_id++);
    unmarked_states.push(start);
    while (!unmarked_states.empty()) {
        auto top = unmarked_states.front();
        unmarked_states.pop();
        int index = visited.at(top);
        set_accepting_state(states[index], top, NFAs);
        for (char c = 1; c < CHAR_MAX; ++c) {// start from 1 since 0 is reserved for EPSILON.
            NFA::Set next = E_closure(Move(top, c));
            if (!visited.count(next)) {
                visited.insert({next, state_id});
                states.emplace_back(state_id++);
                unmarked_states.push(next);
            }
            states[index].transitions.insert({c, visited.at(next)});
        }
    }
}

// Iterate over all NFAs and for each NFA we check if its end_state is in the current_set.
// Set DFA regEXP which maximizes the priority.
void DFA::set_accepting_state(DFA::State &state, const NFA::Set &set, const std::vector<RegularExpression> &NFAs) {
    std::string regEXP;
    int priority = INT_MAX;
    for (const auto &nfa : NFAs) {
        if (set.count(nfa.getNFA().get_end())) {
            if (nfa.getPriority() < priority) {
                priority = nfa.getPriority();
                regEXP = nfa.getName();
            }
        }
    }
    if (priority != INT_MAX) {
        state.regEXP = regEXP;
        state.isAcceptingState = true;
    }
}

void DFA::set_input_stream(std::string input_stream) {

}

Token DFA::get_next_token() {
    return Token();
}

void DFA::minimize_DFA() {

}

const std::vector<DFA::State> &DFA::getStates() const {
    return states;
}

