#include <unistd.h>

#include "eval_ast.h"
#include "execution.h"
#include "options.h"
#include "parser.h"
#include "xalloc.h"

extern struct options *opt;

int exec_pipe(struct ast *left, struct ast *right)
{
    struct pipeline *new_pipe = xcalloc(1, sizeof(struct pipeline));
    pipe(new_pipe->fd);

    int temp_stdout = dup(STDOUT_FILENO);
    dup2(new_pipe->fd[1], STDOUT_FILENO);

    int res1 = eval_ast(left);

    dup2(temp_stdout, STDOUT_FILENO);
    close(temp_stdout);

    int temp_stdin = dup(STDIN_FILENO);
    dup2(new_pipe->fd[0], STDIN_FILENO);

    close(new_pipe->fd[1]);
    eval_ast(right);

    dup2(temp_stdin, STDIN_FILENO);
    close(temp_stdin);

    close(new_pipe->fd[0]);
    close(new_pipe->fd[1]);

    xfree(new_pipe);

    return res1;
}
