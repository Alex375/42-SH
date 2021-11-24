#include "headers/execution.h"

#include <unistd.h>
#include <err.h>

#include "xalloc.h"


void exit_program(char *msg)
{
    xfree_all();
    err(1, "%s", msg);
}

int execute(char *cmd, int fd)
{
    pid_t pid = fork();
    if (pid == -1)
    {
        exit_program("Failed to fork");
    }
    if (pid == 0)
    {
        dup2(fd, STDOUT_FILENO);
        close(fd);
        if (execlp("/bin/sh", cmd, NULL) == -1)
        {
            //TODO : handl exec error
            exit_program("Failed to exec");
        }
    }
    else
    {
        int wstatus;
        close(fd);
        if (waitpid(pid, &wstatus, 0) == -1)
            exit_program("Failed to wait");
        return wstatus;
    }
    exit_program("Failed");
    return -1;
}
