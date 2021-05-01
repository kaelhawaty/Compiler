//
// Created by Abd Elkader on 4/30/2021.
//

#ifndef COMPILER_PARSERUTILS_H
#define COMPILER_PARSERUTILS_H

#include "../Component.h"
#include <string>

std::string getText(component_type c);
std::string trim(const std::string &s);
std::string addSpacesBetweenChars(const std::string &s);
#endif //COMPILER_PARSERUTILS_H
