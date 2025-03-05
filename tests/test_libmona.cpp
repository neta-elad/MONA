#include <iostream>

#include "symboltable.h"
#include "printline.h"
#include "dfa.h"
#include "env.h"
#include "lib.h"
#include "offsets.h"
#include "predlib.h"
#include "untyped.h"
extern "C" {
    #include "mem.h"
}
#include "model.h"

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
    char bString[]{'b', 0};
    Name b{bString, dummyPos};
    const Ident bId = symbolTable.insertVar(&b, Varname0, nullptr);

    ASTForm_Var0 *bVar = new ASTForm_Var0{bId, dummyPos};
    ASTForm_True *trueForm = new ASTForm_True{dummyPos};
    MonaAST *ast = new MonaAST{bVar, trueForm};
    ast->globals.insert(bId);

    codeTable = new CodeTable;
    VarCode formulaCode = ast->formula->makeCode();
    DFA *dfa = formulaCode.DFATranslate();
    formulaCode.remove();

    int numVars = ast->globals.size();

    // copy from symbol table
    char **vnames = new char*[numVars];
    unsigned *offs = new unsigned[numVars];
    char *types = new char[numVars];
    int **univs = new int*[numVars];
    int *trees = new int[numVars];
    IdentList sign, freeVars;
    int ix = 0;
    IdentList::iterator id;
    for (id = ast->globals.begin(); id != ast->globals.end(); id++, ix++) {
        vnames[ix] = symbolTable.lookupSymbol(*id);
        offs[ix] = offsets.off(*id);
        sign.push_back(ix);
        freeVars.push_back(*id);
        switch (symbolTable.lookupType(*id)) {
            case VarnameTree:
                trees[ix] = 1;
            break;
            default:
                trees[ix] = 0;
        }
        IdentList *uu = symbolTable.lookupUnivs(*id);
        if (uu) {
            unsigned j;
            univs[ix] = new int[uu->size()+1];
            for (j = 0; j < uu->size(); j++)
                univs[ix][j] = symbolTable.lookupUnivNumber(uu->get(j));
            univs[ix][j] = -1;
        }
        else
            univs[ix] = nullptr;
        switch (symbolTable.lookupType(*id))
        {
            case Varname0:
                types[ix] = 0;
            break;
            case Varname1:
                types[ix] = 1;
            break;
            default:
                types[ix] = 2;
            break;
        }
    }
    // end copy from symbol table

    char *example = dfaMakeExample(dfa, 1, numVars, offs);

    Model model = buildModelFromExample(example, numVars, vnames, types);
    mem_free(example);

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

    delete[] vnames;
    delete[] offs;
    delete[] types;
    delete[] univs;
    delete[] trees;

    delete ast;

    return 0;
}