#ifndef UTILS_H
#define UTILS_H

#include <string>

#include "symboltable.h"

Ident addVar(std::string_view name_str, MonaTypeTag tag);
Ident addPredicate(std::string_view name_str);

#endif //UTILS_H
