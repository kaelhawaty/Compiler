#include <iostream>
#include <fstream>
#include <regex>
#include "NFAGenerator.h"

#define DEBUG1 1

NFAGenerator::NFAGenerator(string inputFilePath): componentParser(this->regularDefinitions){
    this->inputFilePath = std::move(inputFilePath);
}

vector<RegularExpression> NFAGenerator::getNFAs() {
    this->readInputFile();
    this->addBasicRegularDefinitions();
    for(string& line : this->inputFileLines)
        parseLine(line);
    return getExpressions();
}

vector<RegularExpression> NFAGenerator::getExpressions() {
    return this->regularExpressions;
}

void NFAGenerator::readInputFile() {
    fstream newfile;
    newfile.open(this->inputFilePath,ios::in);
    if (newfile.is_open()){
        string tp;
        while(getline(newfile, tp)){
            this->inputFileLines.push_back(tp);
        }
        newfile.close();
    }
}

void NFAGenerator::addBasicRegularDefinitions() {
    for(char i='a' ; i<= 'z' ;i++)
        addSingleChar(i);

    for(char i='A' ; i<= 'Z' ;i++)
        addSingleChar(i);

    for(char i='0' ; i<= '9' ;i++)
        addSingleChar(i);
    //this->regularDefinitions[string(1, EPSILON)] = this->componentParser.addCharNFA(EPSILON); //{"Lambda"}
    addSingleChar(EPSILON);
}

void NFAGenerator::addSingleChar(char c) {
    string let(1,c);
    this->regularDefinitions[let] = this->componentParser.addCharNFA(c); //{let}
}
// this function is just for debugging ignore it
string getText(component_type c) {
    switch (c){
        case OPEN_BRACKETS: return "OPEN_BRACKETS";
        case CLOSE_BRACKETS: return "CLOSE_BRACKETS";
        case KLEENE_CLOSURE: return "KLEENE_CLOSURE";
        case POS_CLOSURE: return "POS_CLOSURE";
        case OR: return "OR";
        case CONCAT: return "CONCAT";
        case TO: return "TO";
        default: return "RED_DEF";
    }
}
void NFAGenerator::parseLine(string s) {
    if(DEBUG1)cout << s << endl;
    s = trim(s);
    if(s[0] == '['){
        addPunctuations(s);
    }else if(s[0] == '{'){
        addKeywords(s);
    }else{
        int ind = 0;
        while (!(s[ind]=='=' || s[ind]==':')) ind++;
        string name = s.substr(0,ind);
        name = trim(name);
        string expression = s.substr(ind+1);
        expression = trim(expression);
        addRegularDefinition(name,expression);
        if (s[ind] == ':') {
            this->regularExpressions.emplace_back(expression, name, 0, this->regularDefinitions[name]);
        }
    }
}

void NFAGenerator::addRegularDefinition(const string& name, string expression) {
    vector<component> components = getComponents(std::move(expression));
    regularDefinitions[name] = this->componentParser.buildParseTree(components); //{name}

    if(DEBUG1)for(component& c : components)
        cout << getText(c.type) << "   " << (c.type==RED_DEF ? "("+c.regularDefinition+")" : "") << "  ";
    if(DEBUG1)cout << endl << endl;
}

void NFAGenerator::addPunctuations(string s) {

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
    }

}

void NFAGenerator::addKeywords(string s) {
    regex rgx("\\w+");

    for( sregex_iterator it(s.begin(), s.end(), rgx), it_end; it != it_end; ++it ){
        string keyword = (*it)[0];
        string keywordWithSpaces;
        for(char c : keyword){
            keywordWithSpaces.push_back(c);
            keywordWithSpaces.push_back(' ');
        }
        addRegularDefinition(keyword,trim(keywordWithSpaces));
    }

}

vector<component> NFAGenerator::getComponents(string s) {
    vector<component> components;
    for(int i=0 ; i< s.length() ;i++){
        component_type type = getOperationType(s[i]);
        if(type == RED_DEF){
            if( i+1 < s.length() && s[i] == '\\' && s[i+1] == 'L'){
                components.push_back({type,"Lambda"}); //this->regularDefinitions["Lambda"]
                i++;
            }else{
                string name;
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
vector<component> NFAGenerator::filterComponents(vector<component> &components) {
    int size = components.size();
    vector<bool> exist(size, true);
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
    vector<component> filteredComponents;
    for(int i=0 ; i< size ; i++)
        if(exist[i])
            filteredComponents.push_back(components[i]);

    return filteredComponents;
}

component_type NFAGenerator::getOperationType(char c) {
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

string NFAGenerator::trim(const string &s) {
    int start = 0;
    while (s[start] == ' ') start++;
    int end = s.size()-1;
    while (s[end] == ' ') end--;
    return s.substr(start,end-start+1);
}

