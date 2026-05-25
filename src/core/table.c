#include "table.h"

int getLabelHash(const char *label, size_t size)
{
    int result = 0;
    for (int counter = 0; counter < strlen(label); ++counter)
    {
        result = result * 7 + label[counter];
    }
    return result % size;
}

int insertLabel(Symbol *table, size_t size, Symbol value)
{
    int position = getHash(value.label, size);
    while(table[position].used == SYMBOL_USED)
    {
        if(!strncmp(table[position].label, value.label, LABEL_LENGTH))
        {
            return INSERT_FAILED;
        }
        position = (position + 1) % size;
    }
    value.used = SYMBOL_USED;
    table[position] = value;
    return position;
}

int findLabel(Symbol *table, size_t size, const char *label)
{
    int position = getHash(label, size);
    while(table[position].used == SYMBOL_USED)
    {
        if(!strncmp(table[position].label, label, LABEL_LENGTH))
        {
            return position;
        }
        position = (position + 1) % size;
    }
    return FIND_FAILED;
}