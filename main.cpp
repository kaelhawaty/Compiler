#include <iostream>
#include "chrono"
#include "src/Parser/InputParser.h"
#include "src/Parser/Utils/ParserUtils.h"
#include "src/Parser/LexicalParser.h"

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
        if (argc != 3) {
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
        if (!outputFile.is_open()) {
            std::cout << "Error: Couldn't create output file." << "\n";
            return 0;
        }
        while (lexicalParser.get_next_token(token)) {
            outputFile << token.regEXP << "\n";
        }
        outputFile.flush();
        outputFile.close();
    }
    return 0;
}