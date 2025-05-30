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
    Ident pId = addVar("p", Varname0);
    Ident aId = addVar("a", Varname1);
    Ident bId = addVar("b", Varname1);
    Ident cId = addVar("c", Varname1);
    Ident xId = addVar("x", Varname1);
    Ident yId = addVar("y", Varname1);
    Ident sId = addVar("s", Varname2);

    std::unique_ptr<IdentList> abList = std::make_unique<IdentList>(aId);
    abList->insert(bId);

    auto cVar = std::make_shared<ASTTerm1_Var1>(cId);
    ASTFormPtr cIs5 = std::make_shared<ASTForm_Equal1>(
        cVar,
        std::make_shared<ASTTerm1_Int>(5)
    );

    auto aVar = std::make_shared<ASTTerm1_Var1>(aId);
    auto bVar = std::make_shared<ASTTerm1_Var1>(bId);
    auto sVar = std::make_shared<ASTTerm2_Var2>(sId);

    auto pVar = std::make_shared<ASTForm_Var0>(pId);

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

    SharedASTList setList;
    setList.push_back(aVar);
    setList.push_back(bVar);
    setList.push_back(cVar);
    setList.push_back(xVar);
    setList.push_back(yVar);

    ASTTerm2Ptr set = std::make_shared<ASTTerm2_Set>(setList);

    ASTFormPtr equalSets = std::make_shared<ASTForm_Equal2>(sVar, set);

    ASTFormPtr callPred = std::make_shared<ASTForm_Call>(
        pred,
        parList,
        dummyPos
    );

    ASTFormPtr formula = std::make_shared<ASTForm_And>(
        cIs5,
        callPred
    );

    ASTFormPtr extendedFormula = std::make_shared<ASTForm_And>(
        formula,
        equalSets
    );

    ASTFormPtr withP = std::make_shared<ASTForm_And>(
        extendedFormula,
        pVar
    );


    std::unique_ptr<MonaAST> ast = std::make_unique<MonaAST>(withP);
    ast->globals.insert(aId);
    ast->globals.insert(bId);
    ast->globals.insert(cId);
    ast->globals.insert(xId);
    ast->globals.insert(yId);
    ast->globals.insert(sId);
    ast->globals.insert(pId);

    std::optional<Model> model = getModel(*ast);

    if (!model.has_value()) {
        printf("Model is empty\n");
    }

    std::cout << "Booleans:\n";
    for (auto const &[name, value] : model.value().bools) {
        std::cout << name << " = " << (value ? "true" : "false") << std::endl;
    }

    std::cout << "Integers:\n";
    for (auto const &[name, value] : model.value().ints) {
        std::cout << name << " = " << value << std::endl;
    }

    std::cout << "Sets:\n";

    for (auto const &[name, value] : model.value().sets) {
        printf("%s = {", name.c_str());
        for (auto &j: value) {
            printf("%d; ", j);
        }
        printf("}\n");
    }

    predicateLib.remove(pred);

    return 0;
}
