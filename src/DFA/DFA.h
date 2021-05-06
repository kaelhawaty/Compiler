//
// Created by hazem and ziad and mahmoud and kareem on 5/2/2021.
//

#ifndef COMPILER_DFA_H
#define COMPILER_DFA_H

#include <map>
#include <vector>
#include <climits>

#include "../Parser/RegularExpression.h"
#include "../NFA/NFA.h"

class DFA {
public:
    explicit DFA(const std::vector<RegularExpression> &regEXPs);

    struct State {
        explicit State(int id) : id(id), isAcceptingState(false) {
            transitions.resize(CHAR_MAX);
        }

        int id;
        bool isAcceptingState;
        std::string regEXP;
        std::vector<int> transitions;
    };

    const std::vector<State> &getStates() const;

private:

    std::vector<State> states;

    void minimize_DFA();
    std::vector<int> classify();
    std::vector<int> init_classify();
    void reClassify(std::vector<int>& statesClasses);
    std::vector<int> transformTransitions(const std::vector<int> &transitions,
                                          const std::vector<int> &statesClasses);

    static void set_if_accepting_state(State &state, const NFA::Set &set, const std::vector<RegularExpression> &regEXPs);

};


#endif //COMPILER_DFA_H