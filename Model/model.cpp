#include "model.h"

Model buildModelFromExample(char *example, int numVars, char **names, char *types) {
    Model model;
    size_t length = strlen(example)/(numVars + 1);
    int j = 0;
    for (size_t i = 0; i < numVars; i++) {
        switch (types[i]) {
            case 0:
                model[names[i]] = example[i*length] == '1';
            break;
            case 1:
                for (; j < length && example[i*length+j+1] == '0'; j++) {}
            model[names[i]] = j;
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
            model[names[i]] = values;
            break;
        }
    }
    return model;
}