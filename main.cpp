#include <iostream>
#include "src/NFAGenerator.h"
int main()
{
    string inputPath = "C:\\Users\\ziad\\CLionProjects\\Compiler\\input";
    NFAGenerator a = NFAGenerator(inputPath);
    a.getNFA();
}