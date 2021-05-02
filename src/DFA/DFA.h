//
// Created by hazem and ziad and mahmoud and kareem on 5/2/2021.
//

#ifndef COMPILER_DFA_H
#define COMPILER_DFA_H
#include <unordered_map>
#include <vector>
#include "../Parser/RegularExpression.h"

struct Token {
    std::string regEXP;
    std::string match_string;
};


class DFA {
public:
    DFA(std::vector<RegularExpression>);
    void set_input_stream(std::string input_stream);// open l file and store it in string;
    Token get_next_token();
private:
    struct State {
        int id;
        bool isAcceptingState;
        std::string regEXP;
        std::unordered_map<char, int> transitions;
    };
    std::vector<State> states;
    std::string file_stream;
    int cursor;
    void minimize_DFA();

};


#endif //COMPILER_DFA_H