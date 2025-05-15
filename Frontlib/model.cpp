#include "model.h"

#include "codetable.h"
#include "offsets.h"
#include "symboltable.h"

extern "C" {
    #include "mem.h"
}

#define TMP_MODE_OPTIMIZATION

extern SymbolTable symbolTable;
extern CodeTable *codeTable;
extern Offsets offsets;

Model buildModelFromExample(char *example, int numVars, char **names, char *types) {
    Model model;
    size_t length = strlen(example)/(numVars + 1);
    int j = 0;
    for (size_t i = 0; i < numVars; i++) {
        switch (types[i]) {
            case 0:
                model.bools[names[i]] = example[i*length] == '1';
            break;
            case 1:
                for (j = 0; j < length && example[i*length+j+1] == '0'; j++) {}
                model.ints[names[i]] = j;
            break;
            case 2:
                size_t size = 0;
            for (size_t j = 0; j < length; j++) {
                if (example[i*length+j+1] == '1') {
                    ++size;
                }
            }
            std::set<int> values{};

            for (j = 0; j < length; j++) {
                if (example[i*length+j+1] == '1') {
                    values.insert(j);
                }
            }
            model.sets[names[i]] = values;
            break;
        }
    }
    return model;
}

std::optional<Model> getModel(const MonaAST &ast) {
#ifdef TMP_MODE_OPTIMIZATION
    symbolTable.openTmpMode();
#endif

    codeTable = new CodeTable;
    VarCode formulaCode = ast.formula->makeCode();
    DFA *dfa = formulaCode.DFATranslate();
    formulaCode.remove();

    int numVars = ast.globals.size();

    // copy from symbol table
    char **vnames = new char*[numVars];
    unsigned *offs = new unsigned[numVars];
    char *types = new char[numVars];
    int **univs = new int*[numVars];
    int *trees = new int[numVars];
    IdentList sign, freeVars;
    int ix = 0;
    IdentList::iterator id;
    for (id = ast.globals.begin(); id != ast.globals.end(); id++, ix++) {
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

    dfaFree(dfa);

    std::optional<Model> model = {};

    if (example) {
        model = buildModelFromExample(example, numVars, vnames, types);
        mem_free(example);
    }

    delete[] vnames;
    delete[] offs;
    delete[] types;
    delete[] univs;
    delete[] trees;
    delete codeTable;

#ifdef TMP_MODE_OPTIMIZATION
    symbolTable.closeTmpMode();
#endif

    return model;
}
