#include <iostream>
#include "chrono"
#include "src/Parser/InputParser.h"
#include "src/Parser/Utils/ParserUtils.h"
#include "src/Parser/LexicalParser.h"
#include "src/Syntax_Parser/Rules_builder.h"
#include "src/Syntax_Parser/Syntax_parser.h"

#define debug(...) fprintf(stderr, __VA_ARGS__), fflush(stderr)

#define time__(d) \
    for ( \
        auto blockTime = make_pair(chrono::high_resolution_clock::now(), true); \
        blockTime.second; \
        debug("%s: %lld ms\n", d, chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - blockTime.first).count()), blockTime.second = false \
    )


int main(int argc, char *argv[])
{
    using namespace std;
    time__("Execution") {
        if (argc != 4) {
            std::cerr << "Error: You need to specify both the rules file path and program file path." << "\n";
            std::cerr << "Usage: " << argv[0] << " rulesFilePath CFGFilePath programFilePath" << "\n";
            return 0;
        }
        std::string rulesPath{argv[1]};
        LexicalParser lexicalParser(rulesPath);
        if (lexicalParser.has_grammar_error()) {
            std::cerr << "Error: Couldn't Parse Grammar file correctly" << "\n";
            return 0;
        }
        std::string programPath{argv[3]};
        lexicalParser.set_input_stream(programPath);

        std::string cfgPath{argv[2]};
        Rules_builder builder{cfgPath};
        if (builder.fail()) {
            std::cerr << "Failed reading CFG file rules.\n";
            return 0;
        }
        builder.buildLL1Grammar();
        Syntax_parser syn_parser{builder.getRules(), builder.getStartSymbol()};

        std::ofstream outputFile{"output.txt"};
        if (!outputFile.is_open()) {
            std::cerr << "Error: Couldn't create output file." << "\n";
            return 0;
        }
        auto[derivation, status] = syn_parser.parse(lexicalParser);

        std::array<std::string, 3> status_to_string{"Accepted", "Accepted with errors", "Not matched"};
        outputFile << "Syntax parser status: " << status_to_string[static_cast<int>(status)] << "\n";
        for (const std::vector<Symbol> &cur: derivation) {
            for (const Symbol &symbol: cur) {
                outputFile << ((symbol.type == Symbol::Type::TERMINAL) ? ("'" + symbol.name + "'") : symbol.name);
                outputFile << " ";
            }
            outputFile << "\n";
        }
    }
    return 0;
}