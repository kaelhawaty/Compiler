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
    std::vector<int> statesClasses = acceptingRegExpClassify();
    reClassify(statesClasses);
    std::vector<State> newStates;
    //add first state of every class to newStates
    for(int i=0 ; i< states.size() ;i++){
        /* this condition means that this is the first state of class statesClasses[i]
         *first state of different classes are ordered */
        if(statesClasses[i] == newStates.size()){
            newStates.emplace_back(states[i]);
            newStates.back().id = statesClasses[i];
            newStates.back().transitions = transformTransitions(states[i].transitions,statesClasses);
        }
    }
    states = std::move(newStates);
}

//Initial classification based on accepting regular expiration
std::vector<int> DFA::acceptingRegExpClassify() {
    // 0 class for not accepting state.
    // positive class for accepting a certain regular expiration
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

// do iterations on classes of the states until no more classification needed
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

// map each state in vector transitions to its corresponding class
std::vector<int> DFA::transformTransitions(const std::vector<int> &transitions, const std::vector<int> &statesClasses) {
    std::vector<int> transitionClass(CHAR_MAX);
    for(char c = 1 ; c < CHAR_MAX ; c++)
        transitionClass[c] = statesClasses[transitions[c] ];
    return transitionClass;
}

void DFA::set_input_stream(std::string input_stream) {

}

Token DFA::get_next_token() {
    return Token();
}