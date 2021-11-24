#include <sys/wait.h>
#include <unistd.h>

#include "eval_ast.h"
#include "xalloc.h"

static void closefds(int fd[2])
{
    close(fd[0]);
    close(fd[1]);
}


int exec_pipe(char *cmd_left, char **args_left, char *cmd_right,
              char **args_right)
{
    int fd[2];
    pipe(fd);
    int pid = fork();
    if (pid == -1)
        return 1;
    if (pid == 0)
    {
        dup2(fd[1], STDOUT_FILENO);
        closefds(fd);
        if (execlp("/bin/sh", cmd_left, &cmd_left, args_left) == -1)
            //TODO : format execlp
            return 127;
    }
    else
    {
        int wstatus;
        if (waitpid(pid, &wstatus, 0) == -1)
            return 1;
        if (WEXITSTATUS(wstatus) == 127)
            return 1;
        int pid2 = fork();
        if (pid2 == 0)
        {
            dup2(fd[0], STDIN_FILENO);
            closefds(fd);
            if (execlp("/bin/sh", cmd_right, &cmd_right, args_right) == -1)
                //TODO : format execlp
                return 127;
        }
        else
        {
            closefds(fd);
            if (waitpid(pid2, &wstatus, 0) == -1)
                return 1;
            if (WEXITSTATUS(wstatus) == 127)
                return 1;
        }
    }
    return 0;
}
