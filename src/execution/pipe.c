#include "execution.h"


#include <unistd.h>

#include "eval_ast.h"
#include "parser.h"
#include "execution.h"

int exec_pipe(struct ast *left, struct ast *right, struct pipeline *pipeline)
{
    pipe(pipeline->fd);

    pipeline->out = 1;
    int res1 = eval_ast(left, pipeline);

    pipeline->out = 0;
    eval_ast(right, pipeline);

    pipeline->out = -1;
    close(pipeline->fd[0]);
    close(pipeline->fd[1]);

    return res1;
}
