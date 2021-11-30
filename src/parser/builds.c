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

struct ast *build_s_cmd(char *cmd, char **cmd_arg, struct list_var_assign *vars)
{
    struct n_s_cmd *nCmd = xcalloc(1, sizeof(struct n_s_cmd));
    nCmd->cmd = cmd;
    nCmd->cmd_arg = cmd_arg;
    nCmd->vars = vars;

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

    struct ast *res = xcalloc(1, sizeof(struct ast));
    res->type = AST_CMD;
    res->t_ast = cmd;

    return res;
}

struct ast *build_for(char *name, char **seq, struct ast *statement)
{
    struct n_for *nFor = xcalloc(1, sizeof(struct n_for));
    nFor->name = name;
    nFor->seq = seq;
    nFor->statement = statement;

    struct ast *res = xcalloc(1, sizeof(struct ast));
    res->type = AST_FOR;
    res->t_ast = nFor;

    return res;
}
