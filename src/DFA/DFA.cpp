//
// Created by hazem and ziad and mahmoud and kareem on 5/2/2021.
//

#include "DFA.h"
#include <iterator>
#include <sstream>
#include <iostream>

#define INIT_STATE 0
#define FAILURE 0

void DFA::set_input_stream(const std::string& input_stream) {
    this->file_stream.open(input_stream, std::ios::in);
    this->line_number = 0;
}

Token DFA::get_next_token() {
    if (tokenBuffer.empty()) {
        if (!get_next_line()) {
            // Todo: Perform error handling if there's no more tokens.
        }
    }
    Token next_token = std::move(tokenBuffer.front());
    tokenBuffer.pop();
    return next_token;
}

int DFA::get_next_line() {
    if (!this->file_stream.is_open()) {
        return FAILURE;
    }
    std::string line;
    if (getline(this->file_stream, line)) {
        line_number++;
        std::istringstream iss(line);
        std::vector<std::string> words{std::istream_iterator<std::string>{iss},
                                       std::istream_iterator<std::string>{}};

        for (const auto &word: words) {
            performMaximalMunch(word);
        }
        return this->tokenBuffer.size();
    }
    else
        return FAILURE;
}

void DFA::performMaximalMunch(const std::string &word) {
    if (word.empty()) return;

    int lastAcceptingState = -1;
    int lastAcceptingIndex = -1;
    int index = 0;
    while (index < word.length()) {
        State state = this->states[INIT_STATE];
        for (int i = index; i < word.length(); i++) {
            state = this->states.at(state.transitions.at(word[i]));
            if (state.isAcceptingState) {
                lastAcceptingIndex = i;
                lastAcceptingState = state.id;
            }
        }
        if (lastAcceptingIndex == -1) {
            std::cerr << "Error in line " << line_number <<  " :" << word.substr(index) << " Couldn't match\n";
            // skip the first char, and try again from word[index + 1].
            index++;
            continue;
        }

        this->tokenBuffer.push({
            this->states[lastAcceptingState].regEXP,
            word.substr(index, lastAcceptingIndex - index + 1)});
        index = lastAcceptingIndex + 1;
    }
}
