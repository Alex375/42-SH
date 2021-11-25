#include "headers/parser.h"
#include "xalloc.h"
#include "xstrdup.h"

struct ast *build_binary(enum AST_TYPE type, struct ast *left,
                         struct ast *right)
{
    struct n_binary *nBinary = xcalloc(1, sizeof(struct n_binary));
    nBinary->left = left;
    nBinary->right = right;

    struct ast *res = xcalloc(1, sizeof(struct ast));
    res->type = type;
    res->t_ast = nBinary;

    return res;
}

struct ast *build_if(struct ast *condition, struct ast *true_c,
                     struct ast *false_c)
{
    struct n_if *n_if = xcalloc(1, sizeof(struct n_if));
    n_if->condition = condition;
    n_if->true_c = true_c;
    n_if->false_c = false_c;

    struct ast *res = xcalloc(1, sizeof(struct ast));
    res->type = AST_IF;
    res->t_ast = n_if;

    return res;
}

struct ast *build_s_cmd(char *cmd, char **cmd_arg)
{
    struct n_s_cmd *nCmd = xcalloc(1, sizeof(struct n_s_cmd));
    nCmd->cmd = cmd;
    nCmd->cmd_arg = cmd_arg;

    struct ast *res = xcalloc(1, sizeof(struct ast));
    res->type = AST_S_CMD;
    res->t_ast = nCmd;

    return res;
}

struct ast *build_cmd(struct ast *ast, struct list_redir *redirs)
{
    struct n_command *cmd = xcalloc(1, sizeof(struct n_command));
    cmd->ast = ast;
    cmd->redirs = redirs;

    return ast;
}
