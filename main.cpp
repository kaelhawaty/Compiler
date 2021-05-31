#include <iostream>
#include "src/Parser/InputParser.h"
#include "src/Parser/Utils/ParserUtils.h"
#include "src/Parser/LexicalParser.h"
#include "src/Syntax_Parser/Rules_builder.h"
int main(int argc, char *argv[])
{
    /*
    if(argc != 3){
        std::cout << "Error: You need to specify both the rules file path and program file path." << "\n";
        std::cout << "Usage: " << argv[0] << " rulesFilePath programFilePath" << "\n";
        return 0;
    }
    std::string rulesPath{argv[1]};
    LexicalParser lexicalParser(rulesPath);
    if (lexicalParser.has_grammar_error()) {
        std::cerr << "Error: Couldn't Parse Grammar file correctly" << "\n";
        return 0;
    }
    std::string programPath{argv[2]};
    lexicalParser.set_input_stream(programPath);
    Token token;
    std::ofstream outputFile{"output.txt"};
    if(!outputFile.is_open()){
        std::cout << "Error: Couldn't create output file." << "\n";
        return 0;
    }
    while(lexicalParser.get_next_token(token)){
        outputFile << token.regEXP << "\n";
    }
    outputFile.flush();
    outputFile.close();
    */
    std::string in = "cfg.txt";
    Rules_builder builder(in);
    for (const auto &lhs : builder.getRules()) {
        std::cout << "From: " << lhs.first.name << ' ';
        for (const auto &rhs : lhs.second) {
            std::cout << " Group ";
            for (const auto &x : rhs) {
                std::cout << x.name << ' ' << "{ " << (x.type==Symbol::Type::TERMINAL ? "TERMINAL" : (x.type==Symbol::Type::NON_TERMINAL ? "NON_TERMINAL" : "EPSILON")) << " }";
            }
        }
        std::cout << '\n';
    }
    return 0;
}