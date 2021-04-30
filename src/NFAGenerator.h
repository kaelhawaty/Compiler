//
// Created by Abd Elkader on 4/29/2021.
//

#ifndef COMPILER_NFAGENERATOR_H
#define COMPILER_NFAGENERATOR_H
#include <string>
#include <vector>
#include <unordered_map>
#include "NFA.h"
#include "RegularExpression.h"
#include "ComponentParser.h"
#include "Component.h"

using namespace std;


class NFAGenerator {
public:
    NFAGenerator(string inputFilePath);
    vector<RegularExpression> getNFAs();
    vector<RegularExpression> getExpressions();

private:
    string inputFilePath;
    ComponentParser componentParser;
    unordered_map<string,NFA> regularDefinitions;
    vector<string> inputFileLines;
    vector<RegularExpression> regularExpressions;

    void readInputFile();
    void parseLine(string s);
    void addBasicRegularDefinitions();
    vector<component>getComponents(string s);
    component_type getOperationType(char c);
    vector<component> filterComponents(vector<component>& components);
    string trim(const string &s);
    void addKeywords(string s);
    void addPunctuations(string s);
    void addRegularDefinition(const string& name,string expression);
    void addSingleChar(char c);
};


#endif //COMPILER_NFAGENERATOR_H
