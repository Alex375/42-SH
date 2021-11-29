#include "eval_ast.h"
#include <err.h>
#include "execution.h"

int eval_ast(struct ast *ast) // TODO alloc pipline with null
{
    if (!ast)
        return 0;

    struct n_s_cmd *s_cmd_ast;
    struct n_binary *b_ast;
    struct n_if *if_ast;
    struct n_command *cmd;
    struct n_for *for_ast;

    int res = 0;

    switch (ast->type)
    {
    case AST_S_CMD:
        s_cmd_ast = ast->t_ast;
        res = execute(s_cmd_ast->cmd, s_cmd_ast->cmd_arg);
        return res;
    case AST_IF:
        if_ast = ast->t_ast;
        if (eval_ast(if_ast->condition) == 0)
            res = eval_ast(if_ast->true_c);
        else
            res = eval_ast(if_ast->false_c);
        return res;
    case AST_WHILE:
    case AST_UNTIL:
        b_ast = ast->t_ast;
        while ((ast->type == AST_UNTIL && eval_ast(b_ast->left))
                || (ast->type == AST_WHILE
                    && eval_ast(b_ast->left) == 0))
            res = eval_ast(b_ast->right);

        return res;
    case AST_FOR:
        for_ast = ast->t_ast;
        //TODO set var name to for_ast->seq[i] inside for line
        for (int i = 0; for_ast->seq[i]; ++i)
        {
            res = eval_ast(for_ast->statement);
        }

        return res;
    case AST_CASE:
        return 0;
    case AST_PIPE:
        b_ast = ast->t_ast;
        return exec_pipe(b_ast->left, b_ast->right);
    case AST_REDIR:
        return 0;
    case AST_FUNC:
        return 0;
    case AST_LIST:
        b_ast = ast->t_ast;
        eval_ast(b_ast->left);
        return eval_ast(b_ast->right);
    case AST_NOT:
        return !eval_ast(ast->t_ast);
    case AST_AND:
        b_ast = ast->t_ast;
        return eval_ast(b_ast->left)
            || eval_ast(b_ast->right);
    case AST_OR:
        b_ast = ast->t_ast;
        return eval_ast(b_ast->left)
            && eval_ast(b_ast->right);
    case AST_BRACKET:
        return 0;
    case AST_PARENTH:
        return 0;
    case AST_CMD:
        cmd = ast->t_ast;
        // TODO exec redirs
        return eval_ast(cmd->ast);
    default:
        errx(1, "Mettez un default svp ça fait crash cmake ok | j'ai mis un default");
    }
}
