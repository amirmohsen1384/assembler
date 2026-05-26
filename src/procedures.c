#include <string.h>
#include <stdlib.h>
#include "procedures.h"
#include "core/instruction.h"

int analyzeLabels(FILE *file, SymbolTable *table)
{
    size_t pc = 0;
    char buffer[BUFFER_MAX];
    while (fgets(buffer, BUFFER_MAX, file) != NULL)
    {
        InstructionInfo info = parseLine(buffer);
        if (info.label[0] != '\0')
        {
            int position = findByLabel(table, info.label);
            if (position != NOT_FOUND)
            {
                Symbol symbol;
                symbol.pc = pc;
                strncpy(symbol.label, label, LABEL_LENGTH);
                insertSymbol(table, symbol);
            }
            else
            {
                return ANALYSIS_FAILED;
            }
        }
        else if (info.function == Space)
        {
            Word offset = atoi(info.arguments[0]);
            pc += offset;
        }
        else
        {
            pc++;
        }
    }
    return pc;
}