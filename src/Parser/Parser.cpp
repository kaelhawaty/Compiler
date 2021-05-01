//
// Created by Abd Elkader on 5/1/2021.
//


#include "Parser.h"
#include "InputParser.h"
#include "ComponentParser.h"

using namespace std;

vector<RegularExpression> Parser::parse(const string &inputFilePath) {
    InputParser inputParser = InputParser(inputFilePath);
    vector<std::pair<string, vector<component>>> regularDefinitionsComponents =
            inputParser.getRegularDefinitionsComponents();
    vector<string> regularExpressions = inputParser.getRegularExpressions();

    ComponentParser componentParser;
    std::unordered_map<std::string, NFA> regDefToNFA = componentParser.regDefinitionsToNFAs(regularDefinitionsComponents);

    vector<RegularExpression> results;
    int order = 1;
    for (string regExp: regularExpressions) {
        results.emplace_back(regExp, order++, regDefToNFA[regExp]);
    }
    return results;
}

