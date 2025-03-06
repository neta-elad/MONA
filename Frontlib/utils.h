#ifndef UTILS_H
#define UTILS_H

#include <string>

#include "ident.h"
#include "symboltable.h"

Ident addVar(const std::string& name_str, MonaTypeTag tag);

// helper type for the visitor
template<class... Ts>
struct overloaded : Ts... {
    using Ts::operator()...;
};

// explicit deduction guide (not needed as of C++20)
template<class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

#endif //UTILS_H
