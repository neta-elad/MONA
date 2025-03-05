#ifndef MODEL_H
#define MODEL_H

#include <variant>
#include <set>
#include <string>
#include <map>

using Value = std::variant<bool, int, std::set<int>>;
using Model = std::map<std::string, Value>;

Model buildModelFromExample(char *example, int numVars, char **names, char *types);

#endif //MODEL_H
