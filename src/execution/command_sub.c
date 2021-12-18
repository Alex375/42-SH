#include <err.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

#include "eval_ast.h"
#include "xalloc.h"
#include "xparser.h"
#include "xstrdup.h"

int get_stdout(struct ast *ast, char **stdout_r)
{
    int fd[2];
    if (pipe(fd) == -1)
        errx(2, "Failed to pipe");

    int res;
    pid_t pid = fork();

    if (pid == -1)
    {
        xfree_all();
        err(2, "Failed to fork");
    }
    if (pid == 0)
    {
        if (dup2(fd[1], STDOUT_FILENO) == -1)
            errx(2, "Failed to dup2");
        close(fd[0]);
        res = eval_ast(ast);
        exit(res);
    }
    else
    {
        close(fd[1]);
        int wstatus;
        if (waitpid(pid, &wstatus, 0) == -1)
            return 2;
        res = WEXITSTATUS(wstatus);
        char *buff = xcalloc(2049, 1);
        int r;
        int total = 0;
        while ((r = read(fd[0], buff + total, 2048)) > 0)
        {
            total += r;
            buff = xrecalloc(buff, total + 2049);
        }
        close(fd[0]);
        *stdout_r = xstrdup(buff);
        xfree(buff);
    }

    RETURN(res);
}
