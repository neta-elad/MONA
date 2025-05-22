#include "utils.h"

#include <vector>
#include <map>

#include "ident.h"


extern SymbolTable symbolTable;


/**
 * On the one hand, `SymbolTable.insertVar` expects a fresh pointer for every var,
 * that should remain valid for the entire duration of the program;
 * on the other hand, it does not free the memory.
 * Therefore, we add all strings into this vector that will automatically free
 * everything in the end (through the std::vector and std::unique_ptr destructors).
 */

std::vector<std::unique_ptr<std::string> > strings;
std::map<std::string, Ident, std::less<> > stringsToIdents;
std::map<Ident, std::string_view> identsToStrings;

void clear() {
    strings.clear();
    stringsToIdents.clear();
    identsToStrings.clear();
}

Ident addVar(std::string_view name_str, MonaTypeTag tag) {
    if (const auto &iter = stringsToIdents.find(name_str);
        iter != stringsToIdents.end()) {
        return iter->second;
    }
    strings.push_back(std::make_unique<std::string>(name_str));
    auto &name_str_copy = strings.back();
    Name name{name_str_copy->data(), dummyPos};
    Ident ident = symbolTable.insertVar(&name, tag, nullptr);
    stringsToIdents[std::string(name_str)] = ident;
    identsToStrings[ident] = *name_str_copy;
    return ident;
}

Ident addPredicate(std::string_view name_str) {
    if (const auto &iter = stringsToIdents.find(name_str);
        iter != stringsToIdents.end()) {
        return iter->second;
    }
    strings.push_back(std::make_unique<std::string>(name_str));
    auto &name_str_copy = strings.back();
    Name name{name_str_copy->data(), dummyPos};
    Ident ident = symbolTable.insertPred(&name);
    stringsToIdents[std::string(name_str)] = ident;
    identsToStrings[ident] = *name_str_copy;
    return ident;
}

void utils_stats() {
    std::cout
            << strings.size() << " strings, "
            << stringsToIdents.size() << " stringsToIdents, "
            << identsToStrings.size() << " identsToStrings\n";
}
