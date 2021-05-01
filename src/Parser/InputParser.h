//
// Created by Abd Elkader on 4/29/2021.
//

#ifndef COMPILER_INPUTPARSER_H
#define COMPILER_INPUTPARSER_H
#include <string>
#include <vector>
#include <unordered_map>
#include "../NFA/NFA.h"
#include "RegularExpression.h"
#include "ComponentParser.h"
#include "Component.h"



class InputParser {
public:
    explicit InputParser(const std::string& inputFilePath);
    std::vector<std::pair<std::string,std::vector<component>>> getRegularDefinitionsComponents();
    std::vector<std::string> getRegularExpressions();

private:

    std::vector<std::string> regularExpressionsNames;
    std::vector<std::pair<std::string,std::vector<component>>> regularDefinitionsComponents;
    std::set<std::string> regularDefinitionsNames;

    std::vector<std::string> readInputFile(const std::string& inputFilePath);
    void parseLine(std::string s);
    void addBasicRegularDefinitions();
    std::vector<component>getComponents(std::string s);
    component_type getOperationType(char c);
    std::vector<component> filterComponents(std::vector<component>& components);
    void addKeywords(std::string s);
    void addPunctuations(std::string s);
    void addRegularDefinition(const std::string& name,const std::vector<component>& components);
    void addRegularDefinition(const std::string& name,const std::string& expression);
    void addSingleChar(char c);


};


#endif //COMPILER_INPUTPARSER_H
