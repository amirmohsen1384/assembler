#ifndef GENERAL_H
#define GENERAL_H

#define LABEL_LENGTH 8
#define FUNCTION_LENGTH 8

#include <stdint.h>

typedef struct InstructionInfo
{
    int32_t arguments[3];
    char label[LABEL_LENGTH];
    char function[FUNCTION_LENGTH];
}
InstructionInfo;

InstructionInfo parseLine(const char *line);

#endif