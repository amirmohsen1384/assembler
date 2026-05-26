#ifndef TABLE_H
#define TABLE_H

#include "general.h"

typedef struct Symbol
{
    char label[LABEL_LENGTH];
    int32_t isUsed;
    int32_t pc;
}
Symbol;

typedef struct SymbolTable
{
    Symbol* symbols;
    size_t capacity;
    size_t count;
}
SymbolTable;

SymbolTable createSymbolTable(size_t capacity);
void destroySymbolTable(SymbolTable *table);

int insertSymbol(SymbolTable *table, Symbol value);
int findByLabel(SymbolTable *table, const char *label);

#endif