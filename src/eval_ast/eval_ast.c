#include "eval_ast.h"
#include "execution.h"

int eval_ast(struct ast *ast, struct pipeline *pipeline)//TODO alloc pipline with null
{
    if (!ast)
        return 0;

    struct n_cmd *cmd_ast;
    struct n_binary *binary_ast;
    struct n_if *if_ast;

    int res;

    switch (ast->type)
    {
    case AST_CMD:
        cmd_ast = ast->t_ast;
        res = execute(cmd_ast->cmd_line, pipeline);
        if (cmd_ast->cmd_line)
            return 1;
        return res;
    case AST_IF:
        if_ast = ast->t_ast;
        if (eval_ast(if_ast->condition, pipeline))
            res = eval_ast(if_ast->true, pipeline);
        else
            res = eval_ast(if_ast->false, pipeline);
        return res;
    case AST_WHILE:
        return 0;
    case AST_FOR:
        return 0;
    case AST_CASE:
        return 0;
    case AST_UNTIL:
        return 0;
    case AST_PIPE:
        binary_ast = ast->t_ast;
        return exec_pipe(binary_ast->left, binary_ast->right, pipeline);
    case AST_REDIR:
        return 0;
    case AST_FUNC:
        return 0;
    case AST_LIST:
        binary_ast = ast->t_ast;
        eval_ast(binary_ast->left, pipeline);
        return eval_ast(binary_ast->right, pipeline);
    case AST_NOT:
        return !eval_ast(ast->t_ast, pipeline);
    case AST_AND:
        binary_ast = ast->t_ast;
        return eval_ast(binary_ast->left, pipeline) && eval_ast
            (binary_ast->right, pipeline);
    case AST_OR:
        binary_ast = ast->t_ast;
        return eval_ast(binary_ast->left, pipeline) || eval_ast
            (binary_ast->right, pipeline);
    case AST_BRACKET:
        return 0;
    case AST_PARENTH:
        return 0;
    }
}
