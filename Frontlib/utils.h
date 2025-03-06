#ifndef UTILS_H
#define UTILS_H

#include <string>

#include "ident.h"
#include "symboltable.h"

Ident addVar(const std::string& name_str, MonaTypeTag tag);

#endif //UTILS_H
