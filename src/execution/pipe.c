#include <unistd.h>

#include "eval_ast.h"
#include "execution.h"
#include "parser.h"

int exec_pipe(struct ast *left, struct ast *right, struct pipeline *pipeline)
{
    pipe(pipeline->fd);

    int temp_stdout = dup(STDOUT_FILENO);

    pipeline->out = 1;
    int res1 = eval_ast(left, pipeline);

    dup2(temp_stdout, STDOUT_FILENO);
    close(temp_stdout);

    int temp_stdin = dup(STDIN_FILENO);

    pipeline->out = 0;
    eval_ast(right, pipeline);

    dup2(temp_stdin, STDIN_FILENO);
    close(temp_stdin);

    pipeline->out = -1;
    close(pipeline->fd[0]);
    close(pipeline->fd[1]);

    return res1;
}
