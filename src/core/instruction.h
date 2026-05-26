#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#define LABEL_LENGTH 8
#define ARGUMENT_LENGTH 8
#define COMMENT_LENGTH 256

#include <stdint.h>

typedef enum Function {
    UnknownInstruction = -1,
    Add = 0b0000,
    Subtract = 0b0001,
    SetLessThan = 0b0010,
    Or = 0b0011,
    Nand = 0b0100,
    AddImmediate = 0b0101,
    SetLessThanImmediate = 0b0110,
    OrImmediate = 0b0111,
    LoadUpperImmediate = 0b1000,
    LoadWord = 0b1001,
    SaveWord = 0b1010,
    BranchEqual = 0b1011,
    JumpAndLink = 0b1100,
    Jump = 0b1101,
    Halt = 0b1110,
    Fill = 100,
    Space = 101
}
Function;

typedef struct InstructionInfo {
    char arguments[3][ARGUMENT_LENGTH];
    char comment[COMMENT_LENGTH];
    char label[LABEL_LENGTH];
    size_t argumentCount;
    Function function;
}
InstructionInfo;

typedef enum InstructionFormat {
    UnknownFormat = -1,
    Directive,
    IFormat,
    RFormat,
    JFormat
};

enum InstructionFormat getFormat(const InstructionInfo* instruction);
InstructionInfo parseLine(const char* line);

#endif