//
// Created by hazem and ziad and mahmoud and kareem on 5/2/2021.
//

#include "DFA.h"

namespace std
{
    template<> struct hash<vector<int>>
    {
        int operator()(const std::vector<int> &V) const {
            int hash = V.size();
            for(auto &i : V) {
                hash ^= i + 0x9e3779b9 + (hash << 6) + (hash >> 2);
            }
            return hash;
        }
    };
}

const std::vector<DFA::State> &DFA::getStates() const {
    return states;
}

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
        auto current = std::move(unmarked_states.front());
        unmarked_states.pop();
        int index = visited.at(current);
        set_if_accepting_state(states[index], current, regEXPs);
        for (char c = 1; c < CHAR_MAX; ++c) {// start from 1 since 0 is reserved for EPSILON.
            NFA::Set next = E_closure(Move(current, c));
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
void DFA::set_if_accepting_state(DFA::State &state, const NFA::Set &set, const std::vector<RegularExpression> &regEXPs) {
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

void DFA::minimize_DFA() {
    std::vector<int> statesClasses = classify();
    std::vector<State> newStates;
    //Add first state of every class to newStates.
    for(int i=0 ; i< states.size() ;i++){
        // This condition means that this is the first state of class statesClasses[i].
        // First state of different classes are ordered.
        if(statesClasses[i] == newStates.size()){
            newStates.emplace_back(std::move(states[i]));
            newStates.back().id = statesClasses[i];
            newStates.back().transitions = transformTransitions(newStates.back().transitions,statesClasses);
        }
    }
    states = std::move(newStates);
}

std::vector<int> DFA::classify() {
    std::vector<int> statesClasses = init_classify();
    reClassify(statesClasses);
    return statesClasses;
}

/**
 * Initially, we partition the states into two partitions based on if the state is accepting. Then, we further subdivide
 * the accepting states into partitions that accept the same regular expressions since no two states accepting different
 * regular expressions can be merged (Proof by contradiction).
 */
std::vector<int> DFA::init_classify() {
    // Class 0 is for not accepting states.
    // Positive classes are for accepting different regular expression.
    int nextClass = 1;
    std::unordered_map<std::string,int> regExpClass;
    std::vector<int> stateClass(states.size());

    for(int i = 0 ; i< states.size() ; i++){
        if(states[i].isAcceptingState){
            if(regExpClass.find(states[i].regEXP) == regExpClass.end()){
                regExpClass[states[i].regEXP] = nextClass;
                nextClass++;
            }
            stateClass[i] = regExpClass[states[i].regEXP];
        }else{
            stateClass[i] = 0;
        }
    }
    return stateClass;
}

/**
 * The state-minimization algorithm works by partitioning the states of a DFA
 * into groups of states that cannot be distinguished i.e two states s and t
 * are in the same subgroup if and only if for all input symbols a, states s
 * and t have transitions on a to states in the same group. Each group of
 * states is then merged into a single state of the minimum-state DFA. When
 * the partition cannot be refined further by breaking any group into smaller
 * groups, we have the minimum-state DFA.
*/
void DFA::reClassify(std::vector<int> &statesClasses) {
    std::vector<int> newStatesClasses(states.size());
    do{
        int nextClass = 0;
        std::unordered_map<std::vector<int>,int> transitionsClasses;

        for(int i = 0 ; i< states.size() ; i++){
            std::vector<int> transitionClass = transformTransitions(states[i].transitions,statesClasses);
            if(transitionsClasses.count(transitionClass) == 0){
                transitionsClasses[transitionClass] = nextClass;
                nextClass++;
            }
            newStatesClasses[i] = transitionsClasses[transitionClass];
        }

        swap(statesClasses,newStatesClasses);
    }while (statesClasses != newStatesClasses);
}

// Map each state in vector transitions to its corresponding class.
std::vector<int> DFA::transformTransitions(const std::vector<int> &transitions, const std::vector<int> &statesClasses) {
    std::vector<int> transitionClass(CHAR_MAX);
    for(char c = 0 ; c < CHAR_MAX ; c++)
        transitionClass[c] = statesClasses[transitions[c] ];
    return transitionClass;
}

void DFA::set_input_stream(std::string input_stream) {

}

Token DFA::get_next_token() {
    return Token();
}