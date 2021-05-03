//
// Created by hazem and ziad and mahmoud and kareem on 5/2/2021.
//

#ifndef COMPILER_DFA_H
#define COMPILER_DFA_H
#include <unordered_map>
#include <vector>
#include <fstream>

#include "../Parser/RegularExpression.h"

struct Token {
    std::string regEXP;
    std::string match_string;
};


class DFA {
public:
    DFA(std::vector<RegularExpression>);
    void set_input_stream(const std::string& input_stream);
    Token get_next_token();
private:
    struct State {
        int id;
        bool isAcceptingState;
        std::string regEXP;
        std::unordered_map<char, int> transitions;
    };
    std::vector<State> states;
    std::fstream file_stream;
    std::queue<Token> tokenBuffer;
    int line_number;
    void minimize_DFA();
    int get_next_line();
    void performMaximalMunch(const std::string &line);
};


#endif //COMPILER_DFA_H