#include "utils.h"

#include <vector>
#include <map>

extern SymbolTable symbolTable;


/**
 * On the one hand, `SymbolTable.insertVar` expects a fresh pointer for every var,
 * that should remain valid for the entire duration of the program;
 * on the other hand, it does not free the memory.
 * Therefore, we add all strings into this vector that will automatically free
 * everything in the end (through the std::vector and std::unique_ptr destructors).
 */

std::vector<std::unique_ptr<std::string>> strings;
std::map<std::string, Ident> stringsToIdents;

Ident addVar(const std::string& name_str, MonaTypeTag tag) {
    if (stringsToIdents.find(name_str) != stringsToIdents.end()) {
        return stringsToIdents[name_str];
    }
    strings.push_back(std::make_unique<std::string>(name_str));
    auto& name_str_copy = strings.back();
    Name name{name_str_copy->data(), dummyPos};
    Ident ident = symbolTable.insertVar(&name, tag, nullptr);
    stringsToIdents[name_str] = ident;
    return ident;
}

Ident addPredicate(const std::string& name_str) {
    if (stringsToIdents.find(name_str) != stringsToIdents.end()) {
        return stringsToIdents[name_str];
    }
    strings.push_back(std::make_unique<std::string>(name_str));
    auto& name_str_copy = strings.back();
    Name name{name_str_copy->data(), dummyPos};
    Ident ident = symbolTable.insertPred(&name);
    stringsToIdents[name_str] = ident;
    return ident;
}