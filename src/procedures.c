#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "procedures.h"
#include "instruction.h"

int isNumber(char text[])
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

int readNumber(char text[], int *value)
{
    if (!isNumber(text))
    {
        return 0;
    }

    *value = atoi(text);
    return 1;
}

int readRegister(char text[], int *value)
{
    int number = 0;

    if (!readNumber(text, &number))
    {
        return 0;
    }

    if (number < 0 || number > 15)
    {
        return 0;
    }

    *value = number;
    return 1;
}

int getLabelValue(Symbol table[], char label[], int *value)
{
    int position = findByLabel(table, label);

    if (position == NOT_FOUND)
    {
        return 0;
    }

    *value = table[position].pc;
    return 1;
}

int readNumberOrLabel(Symbol table[], char text[], int *value)
{
    if (readNumber(text, value))
    {
        return 1;
    }

    return getLabelValue(table, text, value);
}

int checkSigned16(int value)
{
    if (value < -32768 || value > 32767)
    {
        return 0;
    }

    return 1;
}

int checkUnsigned16(int value)
{
    if (value < 0 || value > 65535)
    {
        return 0;
    }

    return 1;
}

int checkArgs(InstructionInfo info, int count)
{
    if ((int)info.argumentCount != count)
    {
        return 0;
    }

    return 1;
}

int analyzeLabels(FILE *file, Symbol table[])
{
    int pc = 0;
    char buffer[BUFFER_MAX];

    while (fgets(buffer, BUFFER_MAX, file) != NULL)
    {
        InstructionInfo info = parseLine(buffer);

        if (info.function == UnknownInstruction && info.label[0] == '\0')
        {
            continue;
        }

        if (info.label[0] != '\0')
        {
            int position = findByLabel(table, info.label);

            if (position != NOT_FOUND)
            {
                return ANALYSIS_FAILED;
            }

            Symbol symbol = {0};
            symbol.pc = pc;
            symbol.isUsed = SYMBOL_USED;

            strncpy(symbol.label, info.label, LABEL_LENGTH - 1);
            symbol.label[LABEL_LENGTH - 1] = '\0';

            if (insertSymbol(table, symbol) < 0)
            {
                return ANALYSIS_FAILED;
            }
        }

        if (info.function == Space)
        {
            int count = 0;

            if (!checkArgs(info, 1))
            {
                return ANALYSIS_FAILED;
            }

            if (!readNumber(info.arguments[0], &count))
            {
                return ANALYSIS_FAILED;
            }

            if (count < 0)
            {
                return ANALYSIS_FAILED;
            }

            pc += count;
        }
        else
        {
            pc++;
        }
    }

    return pc;
}

int assembleR(InstructionInfo info, Word *code)
{
    int rd = 0;
    int rs = 0;
    int rt = 0;

    if (!checkArgs(info, 3))
    {
        return 0;
    }

    if (!readRegister(info.arguments[0], &rd))
    {
        return 0;
    }

    if (!readRegister(info.arguments[1], &rs))
    {
        return 0;
    }

    if (!readRegister(info.arguments[2], &rt))
    {
        return 0;
    }

    *code = generateRFormatMachineCode(info.function, rd, rs, rt);
    return 1;
}

int assembleI(InstructionInfo info, Symbol table[], int pc, Word *code)
{
    int rs = 0;
    int rt = 0;
    int offset = 0;

    if (info.function == LoadUpperImmediate)
    {
        if (!checkArgs(info, 2))
        {
            return 0;
        }

        if (!readRegister(info.arguments[0], &rt))
        {
            return 0;
        }

        if (!readNumberOrLabel(table, info.arguments[1], &offset))
        {
            return 0;
        }

        if (!checkSigned16(offset))
        {
            return 0;
        }

        rs = 0;
        *code = generateIFormatMachineCode(info.function, rs, rt, offset & 0xFFFF);
        return 1;
    }

    if (info.function == JumpAndLink)
    {
        if (!checkArgs(info, 2))
        {
            return 0;
        }

        if (!readRegister(info.arguments[0], &rt))
        {
            return 0;
        }

        if (!readRegister(info.arguments[1], &rs))
        {
            return 0;
        }

        offset = 0;
        *code = generateIFormatMachineCode(info.function, rs, rt, offset);
        return 1;
    }

    if (!checkArgs(info, 3))
    {
        return 0;
    }

    if (!readRegister(info.arguments[0], &rt))
    {
        return 0;
    }

    if (!readRegister(info.arguments[1], &rs))
    {
        return 0;
    }

    if (info.function == BranchEqual)
    {
        int labelAddress = 0;

        if (!readNumberOrLabel(table, info.arguments[2], &labelAddress))
        {
            return 0;
        }

        offset = labelAddress - pc - 1;

        if (!checkSigned16(offset))
        {
            return 0;
        }

        *code = generateIFormatMachineCode(info.function, rs, rt, offset & 0xFFFF);
        return 1;
    }

    if (!readNumberOrLabel(table, info.arguments[2], &offset))
    {
        return 0;
    }

    if (!checkSigned16(offset))
    {
        return 0;
    }

    *code = generateIFormatMachineCode(info.function, rs, rt, offset & 0xFFFF);
    return 1;
}

int assembleJ(InstructionInfo info, Symbol table[], Word *code)
{
    int address = 0;

    if (info.function == Halt)
    {
        if (!checkArgs(info, 0))
        {
            return 0;
        }

        *code = generateJFormatMachineCode(info.function, 0);
        return 1;
    }

    if (info.function == Jump)
    {
        if (!checkArgs(info, 1))
        {
            return 0;
        }

        if (!readNumberOrLabel(table, info.arguments[0], &address))
        {
            return 0;
        }

        if (!checkUnsigned16(address))
        {
            return 0;
        }

        *code = generateJFormatMachineCode(info.function, address);
        return 1;
    }

    return 0;
}

int assembleDirective(InstructionInfo info, Symbol table[], FILE *output, int *pc)
{
    int value = 0;

    if (info.function == Fill)
    {
        if (!checkArgs(info, 1))
        {
            return 0;
        }

        if (!readNumberOrLabel(table, info.arguments[0], &value))
        {
            return 0;
        }

        fprintf(output, "%d\n", value);
        (*pc)++;
        return 1;
    }

    if (info.function == Space)
    {
        int count = 0;

        if (!checkArgs(info, 1))
        {
            return 0;
        }

        if (!readNumber(info.arguments[0], &count))
        {
            return 0;
        }

        if (count < 0)
        {
            return 0;
        }

        for (int i = 0; i < count; i++)
        {
            fprintf(output, "0\n");
            (*pc)++;
        }

        return 1;
    }

    return 0;
}

int assembleFile(FILE *input, FILE *output, Symbol table[])
{
    char buffer[BUFFER_MAX];
    int pc = 0;

    while (fgets(buffer, BUFFER_MAX, input) != NULL)
    {
        Word code = 0;
        InstructionInfo info = parseLine(buffer);
        FunctionFormat format = getFormat(info.function);

        if (info.function == UnknownInstruction && info.label[0] == '\0')
        {
            continue;
        }

        if (format == UnknownFormat)
        {
            return ANALYSIS_FAILED;
        }

        if (format == Directive)
        {
            if (!assembleDirective(info, table, output, &pc))
            {
                return ANALYSIS_FAILED;
            }

            continue;
        }

        if (format == RFormat)
        {
            if (!assembleR(info, &code))
            {
                return ANALYSIS_FAILED;
            }
        }
        else if (format == IFormat)
        {
            if (!assembleI(info, table, pc, &code))
            {
                return ANALYSIS_FAILED;
            }
        }
        else if (format == JFormat)
        {
            if (!assembleJ(info, table, &code))
            {
                return ANALYSIS_FAILED;
            }
        }
        else
        {
            return ANALYSIS_FAILED;
        }

        fprintf(output, "%u\n", code);
        pc++;
    }

    return 0;
}