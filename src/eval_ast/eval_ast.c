#include "eval_ast.h"

int eval_ast(struct ast *ast)
{
    if (!ast)
        return 0;

    struct n_s_cmd *s_cmd_ast;
    struct n_binary *binary_ast;
    struct n_if *if_ast;
    struct n_command *cmd;

    int res;

    switch (ast->type)
    {
    case AST_S_CMD:
        s_cmd_ast = ast->t_ast;
        //TODO exec cmd
        if (s_cmd_ast->cmd)
            return 1;
        return 0; // exit code of cmd
    case AST_IF:
        if_ast = ast->t_ast;
        if (eval_ast(if_ast->condition))
            res = eval_ast(if_ast->true);
        else
            res = eval_ast(if_ast->false);
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
        binary_ast = ast->t_ast; // TODO REDIR PIPE
        eval_ast(binary_ast->left);
        return eval_ast(binary_ast->right);
    case AST_REDIR:
        return 0;
    case AST_FUNC:
        return 0;
    case AST_LIST:
        binary_ast = ast->t_ast;
        eval_ast(binary_ast->left);
        return eval_ast(binary_ast->right);
    case AST_NOT:
        return !eval_ast(ast->t_ast);
    case AST_AND:
        binary_ast = ast->t_ast;
        return eval_ast(binary_ast->left) && eval_ast(binary_ast->right);
    case AST_OR:
        binary_ast = ast->t_ast;
        return eval_ast(binary_ast->left) || eval_ast(binary_ast->right);
    case AST_BRACKET:
        return 0;
    case AST_PARENTH:
        return 0;
    case AST_CMD:
        cmd = ast->t_ast;
        //TODO exec redirs
        return eval_ast(cmd->ast);
    }
}
