#include <stdio.h>
#include <stdlib.h>
#include "logger.h"
#include "procedures.h"

int main(int argc, char **argv)
{
    initializeLogger();
    if (argc < TOTAL_ARGS)
    {
        logError("You have not entered the input and output file.");
        return EXIT_FAILURE;
    }

    FILE *input_file = fopen(argv[1], "r");
    if (!input_file)
    {
        logError("Failed to open the assembly file.");
        return EXIT_FAILURE;
    }

    Symbol table[TOTAL_CAPACITY] = {0};
    if (analyzeLabels(input_file, table) == ANALYSIS_FAILED)
    {
        fclose(input_file);
        return EXIT_FAILURE;
    }

    rewind(input_file);

    FILE *output_file = fopen(argv[2], "w");
    if (!output_file)
    {
        fclose(input_file);
        logError("Failed to open a file for the output.");
        return EXIT_FAILURE;
    }

    int exitCode = EXIT_SUCCESS;
    if(assembleFile(input_file, output_file, table))
    {
        logError("Failed to open a file for the output.");
        exitCode = EXIT_FAILURE;
    }

    fclose(input_file);
    fclose(output_file);
    logInfo("The program compiled successfully.");
    return exitCode;
}