#include <stdio.h>
#include <string.h>

#include "builtins.h"
#include "options.h"

#define NB_BUILTINS 9

extern struct options *opt;

int get_builins_index(char *function)
{
    if (function == NULL)
        return -1;
    char *builints[NB_BUILTINS] = { "echo",   "exit",  "cd",       "unset", ".",
                                    "export", "break", "continue", "pwd" };
    for (int i = 0; i < NB_BUILTINS; i++)
    {
        if (strcmp(builints[i], function) == 0)
            return i;
    }
    return -1;
}

int exec_builtin(int index, char **args)
{
    if (opt && opt->verbose)
        fprintf(stderr, "Executing builtins\n");
    int (*builints[])(char **) = { echo,   xexit,  cd,        unset, dot,
                                   export, xbreak, xcontinue, pwd };
    return builints[index](args);
}
