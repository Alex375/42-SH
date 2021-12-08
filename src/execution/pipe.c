#include <err.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

#include "eval_ast.h"
#include "execution.h"
#include "options.h"
#include "xalloc.h"
#include "xparser.h"

extern struct options *opt;

int exec_pipe(struct ast *left, struct ast *right)
{
    int fd[2];
    pipe(fd);

    int res = 0;
    pid_t pid1 = fork();

    if (pid1 == -1)
    {
        xfree_all();
        err(2, "Failed to fork");
    }
    if (pid1 == 0)
    {
        dup2(fd[1], STDOUT_FILENO);
        close(fd[0]);
        exit(eval_ast(left));
    }
    else
    {
        close(fd[1]);
        int wstatus;
        if (waitpid(pid1, &wstatus, 0) == -1)
            return 2;
        int pid2 = fork();
        if (pid2 == 0)
        {
            dup2(fd[0], STDIN_FILENO);
            exit(eval_ast(right));
        }
        else
        {
            close(fd[0]);
            if (waitpid(pid2, &wstatus, 0) == -1)
                return 1;
            res = WEXITSTATUS(wstatus);
        }
    }
    return res;
}
