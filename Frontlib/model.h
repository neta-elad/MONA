#ifndef MODEL_H
#define MODEL_H

#include <variant>
#include <set>
#include <string>
#include <map>

#include "ast.h"

using Value = std::variant<bool, int, std::set<int>>;
using Model = std::map<std::string, Value>;

Model getModel(const MonaAST &ast);


#endif //MODEL_H
