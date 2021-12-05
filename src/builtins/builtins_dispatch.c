#include <stdio.h>
#include <string.h>

#include "builtins.h"
#include "options.h"

extern struct options *opt;

int get_builins_index(char *function)
{
    if (function == NULL)
        return -1;
    char *builints[2] = { "echo", "exit" };
    for (int i = 0; i < 2; i++)
    {
        if (strcmp(builints[i], function) == 0)
            return i;
    }
    return -1;
}

int exec_builtin(int index, char **args)
{
    if (opt && opt->verbose)
        fprintf(stdout, "Executing buitdins\n");
    int (*builints[])(char **) = { echo, xexit };
    return builints[index](args);
}
