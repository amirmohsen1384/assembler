#ifndef PROCEDURES_H
#define PROCEDURES_H

#include <stdio.h>
#include "table.h"

int analyzeLabels(FILE *file, Symbol table[]);
int assembleFile(FILE *input, FILE *output, Symbol table[]);
#endif //PROCEDURES_H
