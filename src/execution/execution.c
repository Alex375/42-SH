

#include <err.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>


#include "xalloc.h"

#include "execution.h"
#include "builtins.h"
#include "options.h"

extern struct options *opt;

void exit_program(const char *msg)
{
    xfree_all();
    err(1, "%s", msg);
}

int execute(char *cmd, char **args)
{
    if (!cmd)
        return 0;
    if (opt && opt->verbose)
    {
        fprintf(stderr, "Executing command -> %s\nWith args -> \n", cmd);
    }
    int index;
    if ((index = get_builins_index(cmd)) != -1)
    {
        int res = exec_builtin(index, args);
        return res;
    }
    pid_t pid = fork();
    if (pid == -1)
    {
        exit_program("Failed to fork");
    }
    if (pid == 0)
    {
        if (execvp(cmd, args) == -1)
        {
            fprintf(stderr, "42SH: %s: not found\n", cmd);
            exit(127);
        }
    }
    else
    {
        int wstatus;
        if (waitpid(pid, &wstatus, 0) == -1)
            exit_program("Failed to wait");
        return WEXITSTATUS(wstatus);
    }
    exit_program("Failed");
    return 0;
}
