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

    char input_path[FILE_NAME_MAX];
    strncpy(input_path, argv[1], FILE_NAME_MAX);

    char output_path[FILE_NAME_MAX];
    strncpy(output_path, argv[2], FILE_NAME_MAX);

    FILE* input_file = fopen(input_path, "r");
    if (!input_file)
    {
        fprintf(stderr, "Failed to open the assembly file. Please try again later.");
        return EXIT_FAILURE;
    }

    // Pass the 'input_file' for the first pass

    FILE* output_file = fopen(output_path, "w");
    if (!output_file)
    {
        fprintf(stderr, "Failed to open a file for the output. Please try again later.");
        return EXIT_FAILURE;
    }

    // Pass the 'output_file' for the output

    fclose(input_file);
    fclose(output_file);
    return EXIT_SUCCESS;
}