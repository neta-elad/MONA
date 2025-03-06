#include <iostream>

#include "symboltable.h"
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

int main() {
    std::cout << "Testing libmona\n";
    Ident bId = addVar("b", Varname0);
    Ident i1Id = addVar("i1", Varname1);
    Ident i2Id = addVar("i2", Varname1);
    Ident b2Id = addVar("b2", Varname0);

    IdentList *blist = new IdentList(bId);
    IdentList *blist2 = blist->copy();

    IdentList *i1list = new IdentList(i1Id);
    IdentList *i2list = new IdentList(i2Id);

    ASTFormPtr bVar = std::make_shared<ASTForm_Var0>(bId);
    ASTFormPtr formula = std::make_shared<ASTForm_All1>(
        nullptr,
        i1list,
        std::make_shared<ASTForm_Ex1>(
            nullptr,
            i2list,
            std::make_shared<ASTForm_Less>(
                std::make_shared<ASTTerm1_Var1>(i2Id),
                std::make_shared<ASTTerm1_Var1>(i1Id)
            )
        )
    );
    std::unique_ptr<MonaAST> ast = std::make_unique<MonaAST>(formula);
    ast->globals.insert(bId);
    ast->globals.insert(i1Id);
    ast->globals.insert(i2Id);

    std::optional<Model> model = getModel(*ast);

    if (!model.has_value()) {
        printf("Model is empty\n");
    }

    for (auto const &[name, value]: model.value()) {
        const char *name_cstr = name.c_str();
        std::visit(overloaded{
                       [&name_cstr](bool arg) {
                           printf("Boolean %s = %s\n", name_cstr, arg ? "true" : "false");
                       },
                       [&name_cstr](int arg) {
                           printf("Integer %s = %d\n", name_cstr, arg);
                       },
                       [&name_cstr](const std::set<int> &arg) {
                           printf("Set %s = {", name_cstr);
                           for (auto &j: arg) {
                               printf("%d; ", j);
                           }
                           printf("}\n");
                       }
                   }, value);
    }

    return 0;
}
