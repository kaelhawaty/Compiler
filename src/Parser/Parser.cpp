//
// Created by Abd Elkader on 5/1/2021.
//


#include "Parser.h"
#include "InputParser.h"

using namespace std;

Parser::Parser(const string& inputFilePath) {
    InputParser inputParser = InputParser(inputFilePath);
    std::vector<std::pair<std::string,std::vector<component>>> regularDefinitionsComponents =
            inputParser.getRegularDefinitionsComponents();
    std::vector<std::string> regularExpressions = inputParser.getRegularExpressions();


}
