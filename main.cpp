#include <iostream>
#include "src/Parser/InputParser.h"
#include "src/Parser/Utils/ParserUtils.h"
#include "src/Parser/LexicalParser.h"


int main()
{
    std::string inputPath = "..\\input";
    Parser p;
    p.parse(inputPath);
    return 0;
}