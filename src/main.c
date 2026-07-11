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

    char input_path[BUFFER_MAX];
    strncpy(input_path, argv[1], BUFFER_MAX - 1);
    input_path[BUFFER_MAX - 1] = '\0';

    char output_path[BUFFER_MAX];
    strncpy(output_path, argv[2], BUFFER_MAX - 1);
    output_path[BUFFER_MAX - 1] = '\0';

    FILE *input_file = fopen(input_path, "r");
    if (!input_file)
    {
        fprintf(stderr, "Failed to open the assembly file. Please try again later.");
        return EXIT_FAILURE;
    }

    FILE *output_file = fopen(output_path, "w");
    if (!output_file)
    {
        fclose(input_file);
        fprintf(stderr, "Failed to open a file for the output. Please try again later.");
        return EXIT_FAILURE;
    }

    Symbol table[TOTAL_CAPACITY] = {0};

    if (analyzeLabels(input_file, table) == ANALYSIS_FAILED)
    {
        fclose(input_file);
        fclose(output_file);
        return EXIT_FAILURE;
    }

    rewind(input_file);

    fclose(input_file);
    fclose(output_file);

    return EXIT_SUCCESS;
}