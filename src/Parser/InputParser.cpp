#include <iostream>
#include <fstream>
#include <regex>
#include <climits>
#include "InputParser.h"
#include "Utils/ParserUtils.h"

#define DEBUG1 0
const char EPSILON = 0;

InputParser::InputParser(const std::string& inputFilePath){
    std::vector<std::string> inputFileLines = this->readInputFile(inputFilePath);
    this->addBasicRegularDefinitions();
    for(std::string& line : inputFileLines) if(!line.empty())
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

    for(char i=0 ; i< CHAR_MAX;i++)
        addSingleChar(i);
}

void InputParser::addSingleChar(char c) {
    std::string let(1,c);
    std::vector<component> letComponents(1,{RED_DEF,let});
    addRegularDefinition(let,letComponents);

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
        if (s[ind] == ':')
            this->regularExpressionsNames.emplace_back(name);

        std::string expression = s.substr(ind+1);
        name = trim(name);
        expression = trim(expression);
        addRegularDefinition(name,expression);
    }
}

void InputParser::addRegularDefinition(const std::string &name, const std::string &expression) {
    std::vector<component> components = getComponents(expression);
    addRegularDefinition(name,components);
}

void InputParser::addRegularDefinition(const std::string& name,const std::vector<component>& components) {

    this->regularDefinitionsComponents.emplace_back(name,components);
    this->regularDefinitionsNames.insert(name);

    if(DEBUG1)for(const component& c : components)
            std::cout << getText(c.type) << "   " << (c.type==RED_DEF ? "("+c.regularDefinition+")" : "") << "  ";
    if(DEBUG1)std::cout << std::endl << std::endl;
}

void InputParser::addPunctuations(const std::string& s) {

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

void InputParser::addKeywords(const std::string& s) {
    std::regex rgx("\\w+");

    for( std::sregex_iterator it(s.begin(), s.end(), rgx), it_end; it != it_end; ++it ){
        std::string keyword = (*it)[0];
        std::string keywordWithSpaces = addSpacesBetweenChars(keyword);

        addRegularDefinition(keyword,keywordWithSpaces);
        this->regularExpressionsNames.emplace_back(keyword);
    }

}

std::vector<component> InputParser::getComponents(const std::string& s) {
    std::vector<component> components;
    for(int i=0 ; i< s.length() ;i++){
        component_type type = getOperationType(s[i]);
        if(type == CONCAT) continue;
        if(type == RED_DEF){
            if(!components.empty() && canConcatenate(components.back().type))
                components.emplace_back(CONCAT);
            if( i+1 < s.length() && s[i] == '\\' && s[i+1] == 'L'){
                // 0 is the Regular Definition name for EPSILON
                components.emplace_back(type,std::string(1,0));
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
                //here we check if this regular definition exists, otherwise we split it into letters
                if(this->regularDefinitionsNames.count(name)){
                    components.emplace_back(type,name);
                }else{
                    std::vector<component> lettersComponents = getComponents(addSpacesBetweenChars(name));
                    components.insert(
                            components.end(),
                            std::make_move_iterator(lettersComponents.begin()),
                            std::make_move_iterator(lettersComponents.end())
                            );
                }
                i = j-1;
            }

        }else{
            if(type==OPEN_BRACKETS && !components.empty() && canConcatenate(components.back().type))
                components.emplace_back(CONCAT);

            components.emplace_back(type);
        }
    }
    return components;
}

bool InputParser::canConcatenate(component_type type) {
    return type == RED_DEF || type == CLOSE_BRACKETS || type == KLEENE_CLOSURE || type == POS_CLOSURE;
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

