#include <iostream>
#include <fstream>
#include <regex>
#include <climits>
#include "InputParser.h"
#include "Utils/ParserUtils.h"

#define DEBUG1 1

InputParser::InputParser(const std::string& inputFilePath){
    std::vector<std::string> inputFileLines = this->readInputFile(inputFilePath);
    this->addBasicRegularDefinitions();
    for(std::string& line : inputFileLines)
        parseLine(line);
}

std::vector<std::pair<std::string, std::vector<component>>> InputParser::getRegularDefinitionsComponents() {
    return this->regularDefinitionsComponents;
}

std::vector<std::string> InputParser::getRegularExpressions() {
    return this->regularExpressionsNames;
}

std::vector<std::string> InputParser::readInputFile(const std::string& inputFilePath) {
    std::fstream newfile;
    newfile.open(inputFilePath,std::ios::in);
    std::vector<std::string> inputFileLines;
    if (newfile.is_open()){
        std::string tp;
        while(getline(newfile, tp)){
            inputFileLines.push_back(tp);
        }
        newfile.close();
    }
    return inputFileLines;
}

void InputParser::addBasicRegularDefinitions() {
    for(char i=1 ; i< CHAR_MAX;i++)
        std::cout << i << std::endl,addSingleChar(i);

    // EPSILON = 0 (0 is the Regular Definition name and component for EPSILON )
    addSingleChar(EPSILON);
}

void InputParser::addSingleChar(char c) {
    std::string let(1,c);
    std::vector<component> letComponents(1,{RED_DEF,let});
    this->regularDefinitionsComponents.emplace_back(let,letComponents);
}

void InputParser::parseLine(std::string s) {
    if(DEBUG1)std::cout << s << std::endl;
    s = trim(s);
    if(s[0] == '['){
        addPunctuations(s);
    }else if(s[0] == '{'){
        addKeywords(s);
    }else{
        int ind = 0;
        while (!(s[ind]=='=' || s[ind]==':')) ind++;
        std::string name = s.substr(0,ind);
        name = trim(name);
        std::string expression = s.substr(ind+1);
        expression = trim(expression);
        addRegularDefinition(name,expression);
        if (s[ind] == ':') {
            this->regularExpressionsNames.emplace_back(name);
        }
    }
}

void InputParser::addRegularDefinition(const std::string& name, std::string expression) {
    std::vector<component> components = getComponents(std::move(expression));
    this->regularDefinitionsComponents.emplace_back(name,components);

    if(DEBUG1)for(component& c : components)
            std::cout << getText(c.type) << "   " << (c.type==RED_DEF ? "("+c.regularDefinition+")" : "") << "  ";
    if(DEBUG1)std::cout << std::endl << std::endl;
}

void InputParser::addPunctuations(std::string s) {

    for(int i=1 ; i < s.size()-1 ; i++){
        if(s[i] == ' ') continue;
        char punctuation;
        if(s[i] == '\\'){
            punctuation = s[i+1];
            i++;
        }else{
            punctuation =s[i];
        }
        addSingleChar(punctuation);
        this->regularExpressionsNames.emplace_back(std::string(1,punctuation));
    }

}

void InputParser::addKeywords(std::string s) {
    std::regex rgx("\\w+");

    for( std::sregex_iterator it(s.begin(), s.end(), rgx), it_end; it != it_end; ++it ){
        std::string keyword = (*it)[0];
        std::string keywordWithSpaces;
        for(char c : keyword){
            keywordWithSpaces.push_back(c);
            keywordWithSpaces.push_back(' ');
        }
        addRegularDefinition(keyword,trim(keywordWithSpaces));
        this->regularExpressionsNames.emplace_back(keyword);
    }

}

std::vector<component> InputParser::getComponents(std::string s) {
    std::vector<component> components;
    for(int i=0 ; i< s.length() ;i++){
        component_type type = getOperationType(s[i]);
        if(type == RED_DEF){
            if( i+1 < s.length() && s[i] == '\\' && s[i+1] == 'L'){
                components.push_back({type,std::string(1,0)}); //this->regularDefinitions["Lambda"]
                i++;
            }else{
                std::string name;
                int j = i;
                while (j < s.length() && getOperationType(s[j]) == RED_DEF){
                    if(s[j] == '\\'){
                        name.push_back(s[j+1]);
                        j+=2;
                    }else{
                        name.push_back(s[j]);
                        j+=1;
                    }
                }
                components.push_back({type,name}); //this->regularDefinitions[name]
                i = j-1;
            }

        }else{
            components.push_back({type});
        }
    }
    return filterComponents(components);
}

//removes unnecessary CONCAT operations
std::vector<component> InputParser::filterComponents(std::vector<component> &components) {
    int size = components.size();
    std::vector<bool> exist(size, true);
    for(int i=0 ; i< size ; i++){
        component_type type = components[i].type;
        if( type == KLEENE_CLOSURE || type == POS_CLOSURE || type == CLOSE_BRACKETS || type == OR || type == TO){
            int j = i-1;
            while (j>=0 && components[j].type == CONCAT){
                exist[j] = false;
                j--;
            }
        }
        if(type == OPEN_BRACKETS || type == OR || type == TO) {
            int j = i + 1;
            while (j < size && components[j].type == CONCAT) {
                exist[j] = false;
                j++;
            }
        }
    }
    std::vector<component> filteredComponents;
    for(int i=0 ; i< size ; i++)
        if(exist[i])
            filteredComponents.push_back(components[i]);

    return filteredComponents;
}

component_type InputParser::getOperationType(char c) {
    switch (c){
        case '(': return OPEN_BRACKETS;
        case ')': return CLOSE_BRACKETS;
        case '*': return KLEENE_CLOSURE;
        case '+': return POS_CLOSURE;
        case '|': return OR;
        case ' ': return CONCAT;
        case '-': return TO;
        default: return RED_DEF;
    }
}
