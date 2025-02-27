#include <stdio.h>
#include "dfa.h"

int main() {
    printf("Testing monadfa\n");
    DFA *dfa = dfaTrue();
    dfaPrintVerbose(dfa);
    dfaFree(dfa);
    return 0;
}
