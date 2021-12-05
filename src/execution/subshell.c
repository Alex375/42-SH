#include <err.h>
#include <sys/wait.h>
#include <unistd.h>

#include "eval_ast.h"
#include "xalloc.h"
#include "xparser.h"

int subhsell(struct ast *ast)
{
    int res = 0;
    pid_t pid1 = fork();

    if (pid1 == -1)
    {
        xfree_all();
        err(2, "Failed to fork");
    }
    if (pid1 == 0)
    {
        exit(eval_ast(ast->t_ast));
    }
    else
    {
        int wstatus;
        if (waitpid(pid1, &wstatus, 0) == -1)
            return 2;
        res = WEXITSTATUS(wstatus);
    }
    return res;
}
