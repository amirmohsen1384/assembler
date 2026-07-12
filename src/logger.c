#include <time.h>
#include <stdio.h>
#include "logger.h"
#include <stdarg.h>

static void currentTimestamp(char *buffer, size_t size)
{
    time_t now = time(NULL);
    struct tm *local = localtime(&now);
    strftime(buffer, size, "%Y-%m-%d %H:%M:%S", local);
}

static LoggingLevel readFromFile()
{
    FILE *file = fopen("log.config", "r");
    if (!file)
    {
        file = fopen("log.config", "w");
        if (!file)
        {
            return Debug;
        }
        fprintf(file, "%s=%d", "level", Debug);
        fclose(file);
        return Debug;
    }
    char key[24];
    int value = 0;
    if(fscanf(file, "%23[^=]=%d", key, &value) != 2)
    {
        fclose(file);
        return Debug;
    }
    fclose(file);
    if (value < Debug || value > Error) {
        return Debug;
    }
    return (LoggingLevel)value;
}

static LoggingLevel currentLevel()
{
    return initializeLogger();
}

static void vlog(LoggingLevel level, const char *name, const char *message, va_list arguments)
{
    static const char* level_names[] = {"DEBUG", "INFO", "WARNING", "ERROR"};
    LoggingLevel current = currentLevel();
    if (current > level || level < Debug)
    {
        return;
    }
    char timestamp[48];
    currentTimestamp(timestamp, sizeof(timestamp));
    fprintf(stderr, "%s | [%s] | %s | ", timestamp, level_names[level], name);
    vfprintf(stderr, message, arguments);
    fputc('\n', stderr);
    fflush(stderr);
}

LoggingLevel initializeLogger()
{
    static int initialized = 0;
    static LoggingLevel level;
    if (!initialized)
    {
        level = readFromFile();
        initialized = 1;
    }
    return level;
}

void logWarning(const char *name, const char *message,...)
{
    va_list arguments;
    va_start(arguments, message);
    vlog(Warning, message, name, arguments);
    va_end(arguments);
}

void logError(const char *name, const char *message,...)
{
    va_list arguments;
    va_start(arguments, message);
    vlog(Error, message, name, arguments);
    va_end(arguments);
}

void logDebug(const char *name, const char *message,...)
{
    va_list arguments;
    va_start(arguments, message);
    vlog(Debug, message, name, arguments);
    va_end(arguments);
}

void logInfo(const char *name, const char *message,...)
{
    va_list arguments;
    va_start(arguments, message);
    vlog(Info, message, name, arguments);
    va_end(arguments);
}