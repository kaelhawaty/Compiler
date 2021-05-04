//
// Created by Abd Elkader on 5/1/2021.
//

#ifndef COMPILER_LEXICALPARSER_H
#define COMPILER_LEXICALPARSER_H

#include <string>
#include <fstream>
#include "RegularExpression.h"
#include "../DFA/DFA.h"

struct Token {
    std::string regEXP;
    std::string match_string;
};

class LexicalParser {
public:

    explicit LexicalParser(const std::string &);

    bool get_next_token(Token &);

    void set_input_stream(const std::string &);

private:
    std::fstream file_stream;
    std::queue<Token> tokenBuffer;
    int line_number{};
    const DFA dfa;

    int get_next_line();

    void performMaximalMunch(const std::string &);

    static DFA parse(const std::string &);
};

#endif //COMPILER_LEXICALPARSER_H
