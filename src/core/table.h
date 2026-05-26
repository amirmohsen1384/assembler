#ifndef TABLE_H
#define TABLE_H

#include "general.h"

#define SYMBOL_UNUSED 0
#define SYMBOL_USED 1

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

#define OUT_OF_SPACE -2
#define INSERT_FAILED -1
int insertSymbol(SymbolTable *table, Symbol value);

#define NOT_FOUND -1
int findByLabel(SymbolTable *table, const char *label);

#endif