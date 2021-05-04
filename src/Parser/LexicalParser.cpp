//
// Created by Abd Elkader on 5/1/2021.
//

#include <iterator>
#include <sstream>
#include <iostream>

#include "LexicalParser.h"
#include "InputParser.h"
#include "ComponentParser.h"

LexicalParser::LexicalParser(const std::string &inputFilePath) : dfa(parse(inputFilePath)) {}

/**
 * Coverts Grammar rules stored in a file to Deterministic State Automaton object.
 * @param inputFilePath path to file containing the Grammar of the given language.
 * @return Deterministic State Automaton (DFA object).
 */
DFA LexicalParser::parse(const std::string &inputFilePath) {
    // Parsing Grammar file to have regular definitions.
    InputParser inputParser = InputParser(inputFilePath);
    std::vector<std::pair<std::string, std::vector<component>>> regularDefinitionsComponents =
            inputParser.getRegularDefinitionsComponents();
    std::vector<std::string> regularExpressions = inputParser.getRegularExpressions();

    // Mapping regular definitions to NFA.
    ComponentParser componentParser;
    std::unordered_map<std::string, NFA> regDefToNFA = componentParser.regDefinitionsToNFAs(
            regularDefinitionsComponents);

    std::vector<RegularExpression> results;
    int order = 1;
    for (std::string &regExp: regularExpressions) {
        results.emplace_back(regExp, order++, regDefToNFA[regExp]);
    }

    // Mapping NFA to DFA
    return DFA(results);
}

void LexicalParser::set_input_stream(const std::string &input_stream) {
    this->file_stream.open(input_stream, std::ios::in);
    this->line_number = 0;
}

/**
 * If there's a token, it will be assign it to token parameter then return true,
 * Otherwise return false.
 */
bool LexicalParser::get_next_token(Token &token) {
    // Make sure that there are more tokens to get.
    if (tokenBuffer.empty() && !get_next_line()) {
        return false;
    }
    token = std::move(tokenBuffer.front());
    tokenBuffer.pop();
    return true;
}

/**
 * Tries to get and store more tokens in tokenBuffer.
 * return 0 if no tokens found, number of tokens otherwise.
 */
int LexicalParser::get_next_line() {
    // Make sure that there's an open file to read from.
    if (!this->file_stream.is_open()) {
        return 0;
    }
    std::string line;
    if (getline(this->file_stream, line)) {
        line_number++;
        // Split the line by whitespaces and store all words in a vector.
        std::istringstream iss(line);
        std::vector<std::string> words{std::istream_iterator<std::string>{iss},
                                       std::istream_iterator<std::string>{}};
        for (const auto &word: words) {
            performMaximalMunch(word);
        }
    }
    return this->tokenBuffer.size();
}

void LexicalParser::performMaximalMunch(const std::string &word) {
    if (word.empty()) return;

    int lastAcceptingState = -1;
    int lastAcceptingIndex = -1;
    int index = 0;

    const std::vector<DFA::State> &states = this->dfa.getStates();
    while (index < word.length()) {
        // Assuming that initial state is 0
        const DFA::State *state = &states.at(0);
        for (int i = index; i < word.length(); i++) {
            state = &states.at(state->transitions.at(word[i]));
            if (state->isAcceptingState) {
                // To keep track of the last Accepting state.
                lastAcceptingIndex = i;
                lastAcceptingState = state->id;
            }
        }
        if (lastAcceptingIndex == -1) {
            std::cerr << "Error in line " << line_number << " :" << word.substr(index) << " Couldn't match\n";
            // skip the first char, and try again from word[index + 1].
            index++;
            continue;
        }
        // Store word[index... lastAcceptingIndex] as a token whose state_id is lastAcceptingState.
        this->tokenBuffer.push({states.at(lastAcceptingState).regEXP,
                                word.substr(index, lastAcceptingIndex - index + 1)});
        index = lastAcceptingIndex + 1;
    }
}

