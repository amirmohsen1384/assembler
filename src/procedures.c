#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "logger.h"
#include "procedures.h"
#include "instruction.h"

typedef enum
{
    NoAssemblyError,
    InvalidRegisterNumber,
    UndefinedRegister,
    UndefinedLabel,
    InvalidArguments,
    InvalidOffset,
    UndefinedOpcode,
    NegativeArgument
}
AssemblyError;

static int isNumber(const char* text)
{
    int i = 0;
    if (text[0] == '-' || text[0] == '+')
    {
        i = 1;
    }
    if (text[i] == '\0')
    {
        return 0;
    }

    while (text[i] != '\0')
    {
        if (!isdigit(text[i]))
        {
            return 0;
        }
        i++;
    }
    return 1;
}

static int readNumber(const char* text, int *value)
{
    if (!isNumber(text))
    {
        return 0;
    }
    *value = atoi(text);
    return 1;
}

int analyzeLabels(FILE *file, Symbol table[])
{
    int pc = 0;
    char buffer[BUFFER_MAX];
    int result = EXIT_SUCCESS;
    for (int lineNumber = 1; fgets(buffer, BUFFER_MAX, file) != NULL; ++lineNumber)
    {
        LineParsingError lineError = NoLineParsingError;
        InstructionInfo info = parseLine(buffer, &lineError);
        if (lineError != NoLineParsingError)
        {
            switch (lineError)
            {
                case EmptyLine:
                {
                    logDebug("Label Analysis", "line %d: Identified an empty line", lineNumber);
                    continue;
                }
                case UnknownLineFormat:
                {
                    logError("Label Analysis", "Error at line %d: Found invalid line \"%s\".", lineNumber, buffer);
                    result = EXIT_FAILURE;
                    continue;
                }
                default:
                {
                    logError("Label Analysis", "Error at line %d: Unknown error happened.", lineNumber);
                    result = EXIT_FAILURE;
                    continue;
                }
            }
        }
        else if (info.label[0] != '\0')
        {
            Symbol symbol = {0};
            symbol.pc = pc;
            symbol.isUsed = SYMBOL_USED;
            strncpy(symbol.label, info.label, LABEL_LENGTH - 1);
            symbol.label[LABEL_LENGTH - 1] = '\0';

            SymbolTableError tableError;
            insertSymbol(table, symbol, &tableError);
            if (tableError != NoSymbolTableError)
            {
                switch (tableError)
                {
                    case LabelAlreadyExists:
                    {
                        logError("Label Analysis", "Error at line %d: Label \"%s\" already exists.", lineNumber, symbol.label);
                        result = EXIT_FAILURE;
                        continue;
                    }
                    case FullSymbolTable:
                    {
                        logError("Label Analysis", "Error at line %d: The table has no capacity for \"%s\".", lineNumber, symbol.label);
                        result = EXIT_FAILURE;
                        continue;
                    }
                    default:
                    {
                        logError("Label Analysis", "Error at line %d: Unknown error happened.", lineNumber);
                        result = EXIT_FAILURE;
                        continue;
                    }
                }
            }
        }
        if (info.function == Space)
        {
            int count = 0;
            if (info.argumentCount != 1)
            {
                logError("Label Analysis", "Error at line %d: The entered arguments are %d, but expected 1.", lineNumber, info.argumentCount);
                result = EXIT_FAILURE;
            }
            else if (!readNumber(info.arguments[0], &count))
            {
                logError("Label Analysis", "Error at line %d: The first argument \"%s\" is invalid.", lineNumber, info.arguments[0]);
                result = EXIT_FAILURE;
            }
            else if (count < 0)
            {
                logError("Label Analysis", "Error at line %d: Negative numbers are not accepted", lineNumber);
                result = EXIT_FAILURE;
            }
            pc += count;
        }
        else
        {
            pc++;
        }
    }
    return result;
}

static AssemblyError readRegister(const char* text, int *value)
{
    int number = 0;
    if (!readNumber(text, &number))
    {
        return InvalidRegisterNumber;
    }
    else if (number < 0 || number >= MAX_REGISTERS)
    {
        return UndefinedRegister;
    }
    *value = number;
    return NoAssemblyError;
}

static AssemblyError getLabelValue(Symbol table[], const char* label, int* value)
{
    int position = findByLabel(table, label);
    if (position == NOT_FOUND)
    {
        return UndefinedLabel;
    }
    *value = table[position].pc;
    return NoAssemblyError;
}

static AssemblyError readNumberOrLabel(Symbol table[], const char *text, int *value)
{
    int number = 0;
    if (readNumber(text, &number))
    {
        *value = number;
        return NoAssemblyError;
    }
    return getLabelValue(table, text, value);
}

static int isSigned16(int value)
{
    return value <= 32767 && value >= -32768;
}

static int isUnsigned16(int value)
{
    return value <= 65535 && value >= 0;
}

static AssemblyError assembleR(InstructionInfo info, Word *code)
{
    int rd = 0;
    int rs = 0;
    int rt = 0;
    if (info.argumentCount != 3)
    {
        return InvalidArguments;
    }

    AssemblyError error = readRegister(info.arguments[0], &rd);
    if (error != NoAssemblyError)
    {
        return error;
    }

    error = readRegister(info.arguments[1], &rs);
    if (error != NoAssemblyError)
    {
        return error;
    }

    error = readRegister(info.arguments[2], &rt);
    if (error != NoAssemblyError)
    {
        return error;
    }

    *code = generateRFormatMachineCode(info.function, rd, rs, rt);
    return NoAssemblyError;
}

static AssemblyError assembleI(InstructionInfo info, Symbol table[], int pc, Word *code)
{
    int rs = 0;
    int rt = 0;
    int offset = 0;
    if (info.function == LoadUpperImmediate)
    {
        if (info.argumentCount != 2)
        {
            return InvalidArguments;
        }

        AssemblyError error = readRegister(info.arguments[0], &rt);
        if (error != NoAssemblyError)
        {
            return error;
        }

        error = readNumberOrLabel(table, info.arguments[1], &offset);
        if (error != NoAssemblyError)
        {
            return error;
        }
        else if (!isSigned16(offset))
        {
            return InvalidOffset;
        }
        rs = 0;
        *code = generateIFormatMachineCode(info.function, rs, rt, offset & 0xFFFF);
        return NoAssemblyError;
    }
    else if (info.function == JumpAndLink)
    {
        offset = 0;
        if (info.argumentCount != 2)
        {
            return InvalidArguments;
        }

        AssemblyError error = readRegister(info.arguments[0], &rt);
        if (error != NoAssemblyError)
        {
            return error;
        }

        error = readRegister(info.arguments[1], &rs);
        if (error != NoAssemblyError)
        {
            return error;
        }

        *code = generateIFormatMachineCode(info.function, rs, rt, offset);
        return NoAssemblyError;
    }
    else if (info.argumentCount != 3)
    {
        return InvalidArguments;
    }

    AssemblyError error = readRegister(info.arguments[0], &rt);
    if (error != NoAssemblyError)
    {
        return error;
    }

    error = readRegister(info.arguments[1], &rs);
    if (error != NoAssemblyError)
    {
        return error;
    }

    if (info.function == BranchEqual)
    {
        int labelAddress = 0;
        error = readNumberOrLabel(table, info.arguments[2], &labelAddress);
        if (error != NoAssemblyError)
        {
            return error;
        }
        offset = labelAddress - pc - 1;
        if (!isSigned16(offset))
        {
            return InvalidOffset;
        }
        *code = generateIFormatMachineCode(info.function, rs, rt, offset & 0xFFFF);
        return NoAssemblyError;
    }

    error = readNumberOrLabel(table, info.arguments[2], &offset);
    if (error != NoAssemblyError)
    {
        return error;
    }
    else if (!isSigned16(offset))
    {
        return InvalidOffset;
    }
    *code = generateIFormatMachineCode(info.function, rs, rt, offset & 0xFFFF);
    return NoAssemblyError;
}

static AssemblyError assembleJ(InstructionInfo info, Symbol table[], Word *code)
{
    int address = 0;
    if (info.function == Halt)
    {
        if (info.argumentCount != 0)
        {
            return InvalidArguments;
        }
        *code = generateJFormatMachineCode(info.function, 0);
        return NoAssemblyError;
    }
    else if (info.function == Jump)
    {
        if (info.argumentCount != 1)
        {
            return InvalidArguments;
        }

        AssemblyError error = readNumberOrLabel(table, info.arguments[0], &address);
        if (error != NoAssemblyError)
        {
            return error;
        }
        else if (!isUnsigned16(address))
        {
            return InvalidOffset;
        }
        *code = generateJFormatMachineCode(info.function, address);
        return NoAssemblyError;
    }
    else
    {
        return UndefinedOpcode;
    }
}

static AssemblyError assembleDirective(InstructionInfo info, Symbol table[], FILE *output, int *pc)
{
    int value = 0;
    if (info.function == Fill)
    {
        if (info.argumentCount != 1)
        {
            return InvalidArguments;
        }

        AssemblyError error = readNumberOrLabel(table, info.arguments[0], &value);
        if (error != NoAssemblyError)
        {
            return error;
        }

        fprintf(output, "%d\n", value);
        (*pc)++;
        return NoAssemblyError;
    }
    else if (info.function == Space)
    {
        int count = 0;
        if (info.argumentCount != 1)
        {
            return InvalidArguments;
        }
        if (!readNumber(info.arguments[0], &count))
        {
            return InvalidOffset;
        }
        else if (count < 0)
        {
            return NegativeArgument;
        }
        for (int i = 0; i < count; i++)
        {
            fprintf(output, "0\n");
            (*pc)++;
        }

        return NoAssemblyError;
    }
    else
    {
        return UndefinedOpcode;
    }
}

int assembleFile(FILE *input, FILE *output, Symbol table[])
{
    int pc = 0;
    int result = EXIT_SUCCESS;
    char buffer[BUFFER_MAX];

    for (int lineNumber = 1; fgets(buffer, BUFFER_MAX, input) != NULL; lineNumber++)
    {
        Word code = 0;

        InstructionInfo info = parseLine(buffer, NULL);
        FunctionFormat format = getFormat(info.function);
        AssemblyError error = NoAssemblyError;
        switch (format)
        {
            case RFormat: {
                error = assembleR(info, &code);
                break;
            }
            case IFormat: {
                error = assembleI(info, table, pc, &code);
                break;
            }
            case JFormat: {
                error = assembleJ(info, table, &code);
                break;
            }
            case Directive: {
                error = assembleDirective(info, table, output, &pc);
                if (error == NoAssemblyError) {
                    continue;
                }
                break;
            }
            default: {
                logError("Assembly", "Error at line %d: Unknown instruction.", lineNumber);
                result = EXIT_FAILURE;
                continue;
            }
        }
        if (error != NoAssemblyError)
        {
            result = EXIT_FAILURE;
            switch (error)
            {
                case InvalidArguments: {
                    logError("Assembly", "Error at line %d: Invalid number of arguments", lineNumber);
                    break;
                }
                case InvalidRegisterNumber: {
                    logError("Assembly", "Error at line %d: Found an invalid register number", lineNumber);
                    break;
                }
                case UndefinedRegister: {
                    logError("Assembly", "Error at line %d: Register number is out of range (0-15)", lineNumber);
                    break;
                }
                case UndefinedLabel: {
                    logError("Assembly", "Error at line %d: Undefined label", lineNumber);
                    break;
                }
                case InvalidOffset: {
                    logError("Assembly", "Error at line %d: Offset is outside the 16-bit range", lineNumber);
                    break;
                }
                case NegativeArgument: {
                    logError("Assembly", "Error at line %d: Negative value is not allowed", lineNumber);
                    break;
                }
                case UndefinedOpcode: {
                    logError("Assembly", "Error at line %d: Undefined opcode", lineNumber);
                    break;
                }
                default: {
                    logError("Assembly", "Error at line %d: Unknown assembly error", lineNumber);
                    break;
                }
            }
            continue;
        }
        fprintf(output, "%u\n", code);
        pc++;
    }
    return result;
}