#include <iostream>
#include "src/Parser/InputParser.h"
#include "src/Parser/Utils/ParserUtils.h"
#include "src/Parser/Parser.h"


int main()
{
    std::string inputPath = "..\\input";
    Parser p = Parser(inputPath);
    return 0;
}