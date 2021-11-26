#include <stddef.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

#include "builtins.h"
#include "options.h"
#include "execution.h"

extern struct options *opt;

int is_builins(char *function)
{
    char *builints[1] = { "echo" };
    for (int i = 0; i < 1; i++)
    {
        if (strcmp(builints[i], function) == 0)
            return i;
    }
    return -1;
}

int exec_builtin(int index, char **args, struct pipeline *pipeline)
{
    if (opt->verbose)
        printf("Executing buitdins\n");
    int (*builints[])(char **) = { echo };
    if (pipeline->out != -1)
    {
        printf("%d\n", pipeline->out);
        dup2(pipeline->fd[pipeline->out],
             ((pipeline->out) ? STDOUT_FILENO : STDIN_FILENO));
    }
    return builints[index](args);
}
