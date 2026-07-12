#ifndef ASSEMBLER_LOGGER_H
#define ASSEMBLER_LOGGER_H

typedef enum LoggingLevel {Debug, Info, Warning, Error} LoggingLevel;

void logWarning(const char *message,...);
void logError(const char *message,...);
void logDebug(const char *message,...);
void logInfo(const char *message,...);
LoggingLevel initializeLogger();

#endif //ASSEMBLER_LOGGER_H
