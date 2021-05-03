//
// Created by hazem and ziad and mahmoud and kareem on 5/2/2021.
//

#ifndef COMPILER_DFA_H
#define COMPILER_DFA_H

#include <map>
#include <vector>
#include "../Parser/RegularExpression.h"
#include "../NFA/NFA.h"

struct Token {
    std::string regEXP;
    std::string match_string;
};


class DFA {
public:
    explicit DFA(const std::vector<RegularExpression> &NFAs);

    struct State {
        explicit State(int id) : id(id), isAcceptingState(false) {}

        int id;
        bool isAcceptingState;
        std::string regEXP;
        std::unordered_map<char, int> transitions;
    };

    const std::vector<State> &getStates() const;

    void set_input_stream(std::string input_stream);// open l file and store it in string;
    Token get_next_token();

private:

    std::vector<State> states;
    std::string file_stream;
    int cursor;

    void minimize_DFA();

    static void set_accepting_state(State &state, const NFA::Set &set, const std::vector<RegularExpression> &NFAs);

};


#endif //COMPILER_DFA_H