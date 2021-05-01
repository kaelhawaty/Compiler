//
// Created by Abd Elkader on 5/1/2021.
//

#ifndef COMPILER_PARSER_H
#define COMPILER_PARSER_H

#include <string>
#include "RegularExpression.h"
class Parser {
public:
    std::vector<RegularExpression> parse(const std::string& inputFilePath);
};

#endif //COMPILER_PARSER_H
