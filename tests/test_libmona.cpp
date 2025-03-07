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
    Ident aId = addVar("a", Varname1);
    Ident bId = addVar("b", Varname1);
    Ident cId = addVar("c", Varname1);
    Ident xId = addVar("x", Varname1);
    Ident yId = addVar("y", Varname1);

    std::unique_ptr<IdentList> abList = std::make_unique<IdentList>(aId);
    abList->insert(bId);

    auto cVar = std::make_shared<ASTTerm1_Var1>(cId);
    ASTFormPtr cIs5 = std::make_shared<ASTForm_Equal1>(
        cVar,
        std::make_shared<ASTTerm1_Int>(5)
    );

    auto aVar = std::make_shared<ASTTerm1_Var1>(aId);
    auto bVar = std::make_shared<ASTTerm1_Var1>(bId);

    ASTFormPtr aBetweenBAndC = std::make_shared<ASTForm_And>(
        std::make_shared<ASTForm_Less>(bVar, aVar),
        std::make_shared<ASTForm_Less>(aVar, cVar)
    );

    IdentList *frees = new IdentList, *bound = abList->copy();

    aBetweenBAndC->freeVars(frees, bound);

    Ident pred = addPredicate("a_between_b_and_c");
    predicateLib.insert(
        bound,
        frees,
        abList.release(),
        aBetweenBAndC,
        false,
        pred
    );

    ASTTerm1Ptr xVar = std::make_shared<ASTTerm1_Var1>(xId);
    ASTTerm1Ptr yVar = std::make_shared<ASTTerm1_Var1>(yId);

    SharedASTList parList;
    parList.push_back(xVar);
    parList.push_back(yVar);

    ASTFormPtr callPred = std::make_shared<ASTForm_Call>(
        pred,
        parList,
        dummyPos
    );

    ASTFormPtr formula = std::make_shared<ASTForm_And>(
        cIs5,
        callPred
    );


    std::unique_ptr<MonaAST> ast = std::make_unique<MonaAST>(formula);
    ast->globals.insert(aId);
    ast->globals.insert(bId);
    ast->globals.insert(cId);
    ast->globals.insert(xId);
    ast->globals.insert(yId);

    // std::optional<Model> model;
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
