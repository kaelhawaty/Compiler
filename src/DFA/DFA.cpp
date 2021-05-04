//
// Created by hazem and ziad and mahmoud and kareem on 5/2/2021.
//

#include "DFA.h"

// Construct DFA from list of regular expressions.
DFA::DFA(const std::vector<RegularExpression> &regEXPs) {
    std::queue<NFA::Set> unmarked_states;
    // Maps a given set of NFA nodes to its corresponding DFA state ID.
    std::map<NFA::Set, int> visited;
    // Set of all starting NFA nodes.
    NFA::Set start;
    for (const auto &regEXP : regEXPs) {
        start.insert(regEXP.getNFA().get_start());
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
        set_accepting_state(states[index], top, regEXPs);
        for (char c = 1; c < CHAR_MAX; ++c) {// start from 1 since 0 is reserved for EPSILON.
            NFA::Set next = E_closure(Move(top, c));
            if (!visited.count(next)) {
                visited.insert({next, state_id});
                states.emplace_back(state_id++);
                unmarked_states.push(next);
            }
            states[index].transitions[c] = visited.at(next);
        }
    }
    int empty_set_index = visited.at(NFA::Set());
    for (auto &state : states) {
        state.transitions[0] = empty_set_index;
    }
}

/*
 * Sets the the DFA state to be an accepting state if it contains any accepting NFA nodes. If there are multiple, It picks
 * the regular expression with minimal priority, i.e the earliest regular expression.
 */
void DFA::set_accepting_state(DFA::State &state, const NFA::Set &set, const std::vector<RegularExpression> &regEXPs) {
    std::string regEXP;
    int priority = INT_MAX;
    for (const auto &regEXP : regEXPs) {
        if (!set.count(regEXP.getNFA().get_end())) {
            continue;
        }
        if (regEXP.getPriority() < priority) {
            priority = regEXP.getPriority();
            state.regEXP = regEXP.getName();
            state.isAcceptingState = true;
        }
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

