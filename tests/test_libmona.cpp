#include <iostream>

#include "symboltable.h"
#include "printline.h"
#include "dfa.h"
#include "env.h"
#include "lib.h"
#include "offsets.h"
#include "predlib.h"
#include "untyped.h"

#include "model.h"
#include "utils.h"

Options options;
MonaUntypedAST *untypedAST;
SymbolTable symbolTable(1019);
PredicateLib predicateLib;
Offsets offsets;
CodeTable *codeTable;
Guide guide;
AutLib lib;
int numTypes = 0;
bool regenerate = false;


// helper type for the visitor
template<class... Ts>
struct overloaded : Ts... { using Ts::operator()...; };
// explicit deduction guide (not needed as of C++20)
template<class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;



int main() {
    std::cout << "Testing libmona\n";
    Ident bId = addVar("b", Varname0);
    Ident b2Id = addVar("b2", Varname0);

    ASTFormPtr bVar = std::make_shared<ASTForm_Var0>(bId, dummyPos);
    ASTFormPtr b2Var = std::make_shared<ASTForm_Var0>(b2Id, dummyPos);
    ASTFormPtr andFormula = std::make_shared<ASTForm_And>(bVar, std::make_shared<ASTForm_Not>(b2Var, dummyPos), dummyPos);
    std::unique_ptr<MonaAST> ast = std::make_unique<MonaAST>(andFormula);
    ast->globals.insert(bId);
    ast->globals.insert(b2Id);

    Model model = getModel(*ast);

    if (model.empty()) {
        printf("Model is empty\n");
    }

    for (auto const& [name, value] : model) {
        const char *name_cstr = name.c_str();
        std::visit(overloaded{
            [&name_cstr](bool arg) {
                printf("Boolean %s = %s\n", name_cstr, arg ? "true" : "false");
            },
            [&name_cstr](int arg) {
                printf("Integer %s = %d\n", name_cstr, arg);
            },
            [&name_cstr](const std::set<int>& arg) {
                printf("Set %s = {", name_cstr);
                for (auto& j : arg) {
                    printf("%d; ", j);
                }
                printf("}\n");
            }
        }, value);
    }

    return 0;
}