//
// Created by Abd Elkader on 5/1/2021.
//


#include "Parser.h"
#include "InputParser.h"
#include "ComponentParser.h"


std::vector<RegularExpression> Parser::parse(const std::string &inputFilePath) {
    InputParser inputParser = InputParser(inputFilePath);
    std::vector<std::pair<std::string, std::vector<component>>> regularDefinitionsComponents =
            inputParser.getRegularDefinitionsComponents();
    std::vector<std::string> regularExpressions = inputParser.getRegularExpressions();

    ComponentParser componentParser;
    std::unordered_map<std::string, NFA> regDefToNFA = componentParser.regDefinitionsToNFAs(regularDefinitionsComponents);

    std::vector<RegularExpression> results;
    int order = 1;
    for (std::string& regExp: regularExpressions) {
        results.emplace_back(regExp, order++, regDefToNFA[regExp]);
    }
    return results;
}

