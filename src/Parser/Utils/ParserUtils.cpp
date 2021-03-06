//
// Created by Abd Elkader on 4/30/2021.
//

#include <string>
#include "ParserUtils.h"
// this function is just for debugging
std::string getText(component_type c) {
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


std::string trim(const std::string &s) {
    int start = 0;
    while (s[start] == ' ') start++;
    int end = s.size()-1;
    while (s[end] == ' ') end--;
    return s.substr(start,end-start+1);
}

std::string addSpacesBetweenChars(const std::string &s){
    std::string t;
    for(char c : s){
        t.push_back(c);
        t.push_back(' ');
    }
    t.pop_back();
    return t;
}