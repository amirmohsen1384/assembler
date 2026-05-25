#ifndef TABLE_H
#define TABLE_H

#include "general.h"

#define SYMBOL_UNUSED 0
#define SYMBOL_USED 1

typedef struct Symbol
{
    char label[LABEL_LENGTH];
    int32_t used;
    int32_t pc;
}
Symbol;

int getLabelHash(const char *label, size_t size);

#define INSERT_FAILED -1
int insertLabel(Symbol *table, size_t size, Symbol value);

#define NOT_FOUND -1
int findByLabel(Symbol *table, size_t size, const char *label);

#endif