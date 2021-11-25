#include "execution.h"

#include <unistd.h>
#include <err.h>

#include "xalloc.h"


void exit_program(char *msg)
{
    xfree_all();
    err(1, "%s", msg);
}

int execute(char *cmd, struct pipeline *pipeline)
{
    pid_t pid = fork();
    if (pid == -1)
    {
        exit_program("Failed to fork");
    }
    if (pid == 0)
    {
        if (pipeline->out != -1)
        {
            dup2(pipeline->fd[pipeline->out],
                 ((pipeline->out) ? STDOUT_FILENO : STDIN_FILENO));
            close(pipeline->fd[!(pipeline->out)]);
        }
        if (execlp("/bin/sh", "42sh", "-c", cmd, NULL) == -1)
        {
            // TODO : handle exec error
            err(1, "Failed to exec");
        }
    }
    else
    {
        int wstatus;
        if (!(pipeline->out))
            close(pipeline->fd[1]);
        if (waitpid(pid, &wstatus, 0) == -1)
            exit_program("Failed to wait");
        return wstatus;
    }
    exit_program("Failed");
    return 0;
}
