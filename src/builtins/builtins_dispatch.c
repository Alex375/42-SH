#include <stdio.h>
#include <string.h>

#include "builtins.h"
#include "options.h"

extern struct options *opt;

int get_builins_index(char *function)
{
    char *builints[1] = { "echo" };
    for (int i = 0; i < 1; i++)
    {
        if (strcmp(builints[i], function) == 0)
            return i;
    }
    return -1;
}

int exec_builtin(int index, char **args)
{
    if (opt->verbose)
        fprintf(stderr, "Executing buitdins\n");
    int (*builints[])(char **) = { echo };
    return builints[index](args);
}
