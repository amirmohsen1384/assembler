#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "procedures.h"

int main(int argc, char **argv)
{
    if (argc < TOTAL_ARGS)
    {
        printf("You have not entered the input and output file.");
        return EXIT_FAILURE;
    }

    FILE *input_file = fopen(argv[1], "r");
    if (!input_file)
    {
        fprintf(stderr, "Failed to open the assembly file. Please try again later.");
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
        fprintf(stderr, "Failed to open a file for the output. Please try again later.");
        return EXIT_FAILURE;
    }

    int exitCode = EXIT_SUCCESS;
    if(assembleFile(input_file, output_file, table))
    {
        fprintf(stderr, "Failed to open a file for the output. Please try again later.");
        exitCode = EXIT_FAILURE;
    }
    fclose(input_file);
    fclose(output_file);
    return exitCode;
}