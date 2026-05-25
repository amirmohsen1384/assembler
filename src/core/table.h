#ifndef TABLE_H
#define TABLE_H

#include <string.h>
#include <stdint.h>

#define SYMBOL_USED 1
#define LABEL_LENGTH 8
#define SYMBOL_UNUSED 0

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

#define FIND_FAILED -1
int findByLabel(Symbol *table, size_t size, const char *label);

#endif