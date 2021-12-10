#include "eval_ast.h"

#include <err.h>

#include "ast_info.h"
#include "execution.h"

#define RETURN(val)                                                            \
    res = val;                                                                 \
    set_var_int("?", res);                                                     \
    return res;

#define EVAL_AST(val)                                                          \
    res = eval_ast(val);                                                       \
    set_var_int("?", res);                                                     \
    if (is_breaking())                                                         \
        return res;

#define EVAL_AST_IN_LOOP(val)         \
    inc_depth();\
    res = eval_ast(val);                                                       \
    set_var_int("?", res);                                                     \
    enum ast_info_type t;                                                      \
    if ((t = b_c_is_done()) != A_NOTHING)                                      \
    {                                                                          \
        if (t == A_BREAK)                                                      \
            break;                                                             \
        else                                                                   \
            continue;                                                          \
    }

int eval_ast(struct ast *ast)
{
    if (!ast)
        return 0;

    struct n_s_cmd *s_cmd_ast;
    struct n_binary *b_ast;
    struct n_if *if_ast;
    struct n_command *cmd;
    struct n_for *for_ast;
    struct n_func *func;

    int res = 0;

    switch (ast->type)
    {
    case AST_S_CMD:
        s_cmd_ast = ast->t_ast;

        char **cmd_arg = expand_vars_vect(s_cmd_ast->cmd_arg);
        if (!s_cmd_ast->cmd_arg->len || !cmd_arg || !cmd_arg[0])
        {
            struct list_var_assign *tmp = s_cmd_ast->vars;
            while (tmp)
            {
                char **value = expand_vars_vect(tmp->value);
                add_var(tmp->name, value[0]);
                tmp = tmp->next;
            }
        }
        else
            res = execute(cmd_arg);
        RETURN(res)
    case AST_IF:
        if_ast = ast->t_ast;
        EVAL_AST(if_ast->condition)
        if (res == 0)
        {
            EVAL_AST(if_ast->true_c)
        }
        else
        {
            EVAL_AST(if_ast->false_c)
        }
        RETURN(res)
    case AST_WHILE:
    case AST_UNTIL:
        b_ast = ast->t_ast;

        if (ast->type == AST_UNTIL)
        {
            EVAL_AST_IN_LOOP()
        }
        while ((ast->type == AST_UNTIL && eval_ast(b_ast->left))
               || (ast->type == AST_WHILE && eval_ast(b_ast->left) == 0))
        {
            EVAL_AST_IN_LOOP(b_ast->right)
        }

        RETURN(res)
    case AST_FOR:
        for_ast = ast->t_ast;
        char **seq = expand_vars_vect(for_ast->seq);
        for (int i = 0; seq[i]; ++i)
        {
            add_var(for_ast->name, seq[i]);
            EVAL_AST(for_ast->statement);
        }

        RETURN(res)
    case AST_CASE:
        RETURN(0)
    case AST_PIPE:
        b_ast = ast->t_ast;
        RETURN(exec_pipe(b_ast->left, b_ast->right))
    case AST_SUBSHELL:
        RETURN(subhsell(ast))
    case AST_FUNC:
        func = ast->t_ast;
        add_fc(func->name, func->ast);
        RETURN(0)
    case AST_LIST:
        b_ast = ast->t_ast;
        eval_ast(b_ast->left);
        RETURN(eval_ast(b_ast->right))
    case AST_NOT:
        RETURN(!eval_ast(ast->t_ast))
    case AST_AND:
        b_ast = ast->t_ast;
        RETURN(eval_ast(b_ast->left) || eval_ast(b_ast->right))
    case AST_OR:
        b_ast = ast->t_ast;
        RETURN(eval_ast(b_ast->left) && eval_ast(b_ast->right))
    case AST_BRACKET:
        RETURN(0)
    case AST_PARENTH:
        RETURN(0)
    case AST_CMD:
        cmd = ast->t_ast;
        RETURN(exec_redirs(cmd->ast, cmd->redirs))
    default:
        errx(1,
             "Mettez un default svp ça fait crash cmake ok | j'ai mis un "
             "default | nan cest moi qui met le default");
    }
}
