#ifndef MODEL_H
#define MODEL_H

#include <set>
#include <string>
#include <map>
#include <optional>

#include "ast.h"


struct Model {
    std::map<std::string, bool, std::less<>> bools;
    std::map<std::string, int, std::less<>> ints;
    std::map<std::string, std::set<int>, std::less<>> sets;
};

std::optional<Model> getModel(const MonaAST &ast);


#endif //MODEL_H
