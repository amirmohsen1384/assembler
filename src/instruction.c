#include "instruction.h"
#include <string.h>
#include <ctype.h>

static void copyToken(char *dest, size_t size, const char *src)
{
    if (dest == NULL || size == 0)
    {
        return;
    }

    if (src == NULL)
    {
        dest[0] = '\0';
        return;
    }

    strncpy(dest, src, size - 1);
    dest[size - 1] = '\0';
}

static void trim(char *s)
{
    char *start = s;
    char *end;

    if (s == NULL)
    {
        return;
    }

    while (*start && isspace((unsigned char)*start))
    {
        ++start;
    }

    if (start != s)
    {
        memmove(s, start, strlen(start) + 1);
    }

    end = s + strlen(s);
    while (end > s && isspace((unsigned char)*(end - 1)))
    {
        --end;
    }

    *end = '\0';
}

static Function parseFunctionName(const char *token)
{
    if (strcmp(token, "add") == 0)
    {
        return Add;
    }
    if (strcmp(token, "sub") == 0)
    {
        return Subtract;
    }
    if (strcmp(token, "slt") == 0)
    {
        return SetLessThan;
    }
    if (strcmp(token, "or") == 0)
    {
        return Or;
    }
    if (strcmp(token, "nand") == 0)
    {
        return Nand;
    }
    if (strcmp(token, "addi") == 0)
    {
        return AddImmediate;
    }
    if (strcmp(token, "slti") == 0)
    {
        return SetLessThanImmediate;
    }
    if (strcmp(token, "ori") == 0)
    {
        return OrImmediate;
    }
    if (strcmp(token, "lui") == 0)
    {
        return LoadUpperImmediate;
    }
    if (strcmp(token, "lw") == 0)
    {
        return LoadWord;
    }
    if (strcmp(token, "sw") == 0)
    {
        return SaveWord;
    }
    if (strcmp(token, "beq") == 0)
    {
        return BranchEqual;
    }
    if (strcmp(token, "jalr") == 0)
    {
        return JumpAndLink;
    }
    if (strcmp(token, "j") == 0)
    {
        return Jump;
    }
    if (strcmp(token, "halt") == 0)
    {
        return Halt;
    }
    if (strcmp(token, ".fill") == 0)
    {
        return Fill;
    }
    if (strcmp(token, ".space") == 0)
    {
        return Space;
    }
    return UnknownInstruction;
}

FunctionFormat getFormat(const Function function)
{
    switch (function) {
        case Add:
        case Subtract:
        case SetLessThan:
        case Or:
        case Nand: {
            return RFormat;
        }
        case AddImmediate:
        case SetLessThanImmediate:
        case OrImmediate:
        case LoadUpperImmediate:
        case LoadWord:
        case SaveWord:
        case BranchEqual:
        case JumpAndLink: {
            return IFormat;
        }
        case Jump:
        case Halt: {
            return JFormat;
        }
        case Fill:
        case Space: {
            return Directive;
        }
        default: {
            return UnknownFormat;
        }
    }
}

InstructionInfo parseLine(const char *line)
{
    InstructionInfo result;
    char buffer[BUFFER_MAX];
    char *tokens[5] = {NULL, NULL, NULL, NULL, NULL};
    char *commentStart = NULL;
    size_t tokenCount = 0;
    char *p = NULL;

    memset(&result, 0, sizeof(result));
    result.function = UnknownInstruction;
    result.argumentCount = 0;
    if (line == NULL)
    {
        return result;
    }

    copyToken(buffer, sizeof(buffer), line);
    {
        char *hash = strchr(buffer, '#');
        commentStart = NULL;
        if (hash != NULL)
        {
            commentStart = hash;
        }
        if (commentStart != NULL)
        {
            copyToken(result.comment, sizeof(result.comment), commentStart);
            *commentStart = '\0';
        }
    }
    trim(buffer);
    if (buffer[0] == '\0')
    {
        return result;
    }

    p = strtok(buffer, " \t,\r\n");
    while (p != NULL && tokenCount < 5)
    {
        tokens[tokenCount++] = p;
        p = strtok(NULL, " \t,\r\n");
    }

    if (tokenCount == 0)
    {
        return result;
    }

    result.function = parseFunctionName(tokens[0]);

    if (result.function != UnknownInstruction)
    {
        for (size_t i = 1; i < tokenCount && result.argumentCount < 3; ++i)
        {
            copyToken(result.arguments[result.argumentCount],
                      sizeof(result.arguments[result.argumentCount]),
                      tokens[i]
            );
            ++result.argumentCount;
        }
    }
    else
    {
        copyToken(result.label, sizeof(result.label), tokens[0]);
        if (tokenCount >= 2)
        {
            result.function = parseFunctionName(tokens[1]);
            for (size_t i = 2; i < tokenCount && result.argumentCount < 3; ++i)
            {
                copyToken(result.arguments[result.argumentCount],
                          sizeof(result.arguments[result.argumentCount]),
                          tokens[i]
                );
                ++result.argumentCount;
            }
        }
    }
    return result;
}

Word generateJFormatMachineCode(Function function, Byte rt)
{
    Word opcode = (Word) function;
    return (opcode << 24) | rt;
}

Word generateRFormatMachineCode(Function function, Word rd, Word rs, Word rt)
{
    Word opcode = (Word) function;
    return (opcode << 24) | (rs << 20) | (rt << 16) | (rd << 12);
}

Word generateIFormatMachineCode(Function function, Word rs, Word rt, Word offset)
{
    Word opcode = (Word) function;
    return (opcode << 24) | (rs << 20) | (rt << 16) | offset;
}