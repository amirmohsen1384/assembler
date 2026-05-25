#include "general.h"
#include <stdio.h>

InstructionInfo parseLine(const char *line)
{
    InstructionInfo result;
    result.label[0] = '\0';
    result.arguments[0] = 0;
    result.arguments[1] = 0;
    result.arguments[2] = 0;
    result.function[0] = '\0';
    int tokens = sscanf(line, "%s %s %d,%d,%d",
           result.label,
           result.function,
           result.arguments[0],
           result.arguments[1],
           result.arguments[2]
    );
    return result;
}