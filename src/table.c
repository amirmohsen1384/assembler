#include "table.h"
#include <string.h>

int getLabelHash(const char *label, size_t capacity)
{
    int result = 0;
    for (int i = 0; label[i] != '\0'; i++)
    {
        result = result * 7 + label[i];
    }
    return result % capacity;
}

int insertSymbol(Symbol table[], Symbol value)
{
    int position = getLabelHash(value.label, TOTAL_CAPACITY);
    for (int i = 0; i < TOTAL_CAPACITY; i++)
    {
        if (table[position].isUsed == SYMBOL_UNUSED)
        {
            value.isUsed = SYMBOL_USED;
            table[position] = value;
            return position;
        }

        if (strncmp(table[position].label, value.label, LABEL_LENGTH) == 0)
        {
            return INSERT_FAILED;
        }

        position = (position + 1) % TOTAL_CAPACITY;
    }

    return OUT_OF_SPACE;
}

int findByLabel(Symbol table[], const char *label)
{
    int position = getLabelHash(label, TOTAL_CAPACITY);
    for (int i = 0; i < TOTAL_CAPACITY; i++)
    {
        if (table[position].isUsed == SYMBOL_UNUSED)
        {
            return NOT_FOUND;
        }

        if (strncmp(table[position].label, label, LABEL_LENGTH) == 0)
        {
            return position;
        }
        position = (position + 1) % TOTAL_CAPACITY;
    }

    return NOT_FOUND;
}