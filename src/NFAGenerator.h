//
// Created by Abd Elkader on 4/29/2021.
//

#ifndef COMPILER_NFAGENERATOR_H
#define COMPILER_NFAGENERATOR_H
#include <string>
#include <vector>
#include <unordered_map>
#include "NFANode.h"
#include "RegularDefinition.h"

using namespace std;
enum component_type {
    REG_EXP,
    RED_DEF,
    EQUAL,
    CLOSE_BRACKETS,
    OPEN_BRACKETS,
    POS_CLOSURE,
    KLEENE_CLOSURE,
    CONCAT,
    OR,
    TO
};
struct component {
    component_type type;
    RegularDefinition regularDefinition;
};

class NFAGenerator {
public:
    NFAGenerator(string inputFilePath);
    NFANode getNFA();

private:
    string inputFilePath;
    unordered_map<string,RegularDefinition> regularDefinitions;
    vector<string> inputFileLines;

    void readInputFile();
    void parseLine(string s);
    void addBasicRegularDefinitions();
    vector<component>getComponents(string s);
    component_type getOperationType(char c);
    vector<component> filterComponents(vector<component>& components);
    string trim(const string &s);
    void addKeywords(string s);
    void addPunctuations(string s);
    void addRegularDefinition(string name,string expression);
    void addSingleChar(char c);
};


#endif //COMPILER_NFAGENERATOR_H
