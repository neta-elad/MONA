#include <stdio.h>
#include "gta.h"

Guide guide;

int main() {
    printf("Testing monagta\n");
    GTA* gta = gtaTrue();
    gtaPrintVerbose(gta);
    gtaFree(gta);
    return 0;
}