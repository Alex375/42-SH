#include <err.h>
#include <stdio.h>
#include <unistd.h>

#include "eval_ast.h"
#include "xalloc.h"
#include "xparser.h"

int get_stdout(struct ast *ast, char **stdout_r)
{
    int fd[2];
    pipe(fd);

    int res = 0;
    pid_t pid = fork();

    if (pid == -1)
    {
        xfree_all();
        err(2, "Failed to fork");
    }
    if (pid == 0)
    {
        dup2(fd[1], STDOUT_FILENO);
        close(fd[0]);
        res = eval_ast(ast);
        exit(res);
    }
    else
    {
        close(fd[1]);
        char *buff = xcalloc(2049, 1);
        int r = 0;
        int total = 0;
        while ((r = read(2048, buff + total, fd[0])) > 0)
        {
            total += r;
            buff = xrecalloc(buff, total + 2049);
        }
        *stdout_r = buff;
    }
    return res;
}
