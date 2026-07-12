#ifndef GENERAL_H
#define GENERAL_H

#include <stdint.h>

#define LABEL_LENGTH 7
#define ARGUMENT_LENGTH 32
#define COMMENT_LENGTH 256

#define SYMBOL_UNUSED 0
#define SYMBOL_USED 1

#define NOT_FOUND -1
#define INVALID_POSITION -1

#define TOTAL_ARGS 3
#define BUFFER_MAX 256

#define TOTAL_CAPACITY 101

typedef uint32_t Word;
typedef uint8_t Byte;

#endif // GENERAL_H
