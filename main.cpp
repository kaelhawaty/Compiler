#include <iostream>
#include "src/Parser/InputParser.h"
#include "src/Parser/Utils/ParserUtils.h"
#include "src/Parser/LexicalParser.h"
#include "src/Syntax_Parser/CFG_Reader.h"
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
    auto rules = CFG_Reader::parse_input_file(in);
    for (auto lhs : std::get<0>(rules)) {
        std::cout << "From: " << lhs.first.name << ' ';
        for (auto rhs : lhs.second) {
            std::cout << " Group ";
            for (auto x : rhs) {
                std::cout << x.name << ' ' << "{ " << (x.type==Symbol::Type::TERMINAL ? "TERMINAL" : (x.type==Symbol::Type::NON_TERMINAL ? "NON_TERMINAL" : "EPSILON")) << " }";
            }
        }
        std::cout << '\n';
    }
    return 0;
}