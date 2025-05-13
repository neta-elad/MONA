#ifndef UTILS_H
#define UTILS_H

#include <string>

#include "symboltable.h"

void clear();
Ident addVar(std::string_view name_str, MonaTypeTag tag);
Ident addPredicate(std::string_view name_str);
void utils_stats();

#endif //UTILS_H
