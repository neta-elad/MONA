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

typedef struct {
    size_t size;
    int elements[];
} IntArray;

typedef union {
    bool b;
    int i;
    IntArray *a;
} Value;

typedef struct {
    size_t size;
    Value values[];
} Model;

IntArray *makeIntArray(size_t size) {
    IntArray *intArray = static_cast<IntArray *>(mem_alloc(sizeof(size_t) + sizeof(int) * size));
    intArray->size = size;
    return intArray;
}

Model *makeModel(size_t size) {
    Model *model = static_cast<Model *>(mem_alloc(sizeof(size_t) + sizeof(Value) * size));
    model->size = size;
    return model;
}

void freeModel(Model *model, char *types) {
    for (size_t i = 0; i < model->size; i++) {
        if (types[i] == 2) {
            mem_free(model->values[i].a);
        }
    }
    mem_free(model);
}

Model *buildModelFromExample(char *example, int numVars, char *types) {
    Model *model = makeModel(numVars);
    size_t length = strlen(example)/(numVars + 1);
    int j = 0, k = 0;
    for (size_t i = 0; i < numVars; i++) {
        switch (types[i]) {
            case 0:
                model->values[i].b = example[i*length] == '1';
                break;
            case 1:
                for (; j < length && example[i*length+j+1] == '0'; j++) {}
                model->values[i].i = j;
                break;
            case 2:
                size_t size = 0;
                for (size_t j = 0; j < length; j++) {
                    if (example[i*length+j+1] == '1') {
                        ++size;
                    }
                }
                model->values[i].a = makeIntArray(size);

                for (j = 0, k = 0; j < length; j++) {
                    if (example[i*length+j+1] == '1') {
                        model->values[i].a->elements[k++] = j;
                    }
                }
                break;
        }
    }
    return model;
}


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

    Model *model = buildModelFromExample(example, numVars, types);
    mem_free(example);

    for (int i = 0; i < numVars; i++) {
        switch (types[i]) {
            case 0:
                printf("Boolean %s = %s\n", vnames[i], model->values[i].b ? "true" : "false");
                break;
            case 1:
                printf("Integer %s = %d\n", vnames[i], model->values[i].i);
                break;
            case 2:
                printf("Set %s = {", vnames[i]);
                for (int j = 0; j < model->values[i].a->size; j++) {
                    if (j != 0) {
                        printf(", ");
                    }
                    printf("%d", model->values[i].a->elements[j]);
                }
            printf("}\n");
        }
    }

    freeModel(model, types);

    delete[] vnames;
    delete[] offs;
    delete[] types;
    delete[] univs;
    delete[] trees;

    delete ast;

    return 0;
}