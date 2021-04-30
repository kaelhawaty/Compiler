#include <iostream>
#include "src/NFAGenerator.h"
int main()
{
    string inputPath = "F:\\Compiler\\Compiler\\input";
    NFAGenerator a = NFAGenerator(inputPath);
    a.getNFAs();
}