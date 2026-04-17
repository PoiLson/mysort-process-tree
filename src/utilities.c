#include "main.h"


void getFlags(int argc, char* argv[], int* k, const char** filename, const char** sorting1, const char** sorting2)
{

    for (int idx = 1; idx < argc; idx++)
    {
        char* argument = argv[idx];

        if (strcmp(argument, "-i") == 0)
        {
            idx++;
            *filename = argv[idx];
        }

        if (strcmp(argument, "-k") == 0)
        {
            idx++;
            *k = atoi(argv[idx]);
        }

        if (strcmp(argument, "-e1") == 0)
        {
            idx++;
            *sorting1 = argv[idx];
        }

        if (strcmp(argument, "-e2") == 0)
        {
            idx++;
            *sorting2 = argv[idx];
        }
    }

    if (*filename == NULL)
    {
        printf("No flag for file was inserted!\n");
        exit(-1);
    }


    if ((*k) == 0)
    {
        printf("No flag for bucket entries was inserted!\n");
        exit(-1);
    }

    if (*sorting1 == NULL)
    {
        printf("No flag for sorting number1 was inserted!\n");
        exit(-1);
    }

    if (*sorting2 == NULL)
    {
        printf("No flag for sorting number2 was inserted!\n");
        exit(-1);
    }
}




int openPipeForWrite(const char* fifo_path)
{
    int fifo_fd = open(fifo_path, O_WRONLY);
    if(fifo_fd == -1)
    {
        printf("An error occured with openPipeForWrite(%s) -> %s\n", fifo_path, strerror(errno));
        exit(EXIT_FAILURE);
    }

    return fifo_fd;
}