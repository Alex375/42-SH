#include "handle_ast.h"

#include <stddef.h>

#include "ast_xalloc.h"
#include "vector_tokens.h"
#include "xparser.h"
#include "xstrdup.h"

static void free_list_assign(struct list_var_assign *assign)
{
    if (!assign)
        return;

    free_list_assign(assign->next);

    xfree(assign->name);
    free_token_vect(assign->value);
    xfree(assign);
}

static void free_list_redir(struct list_redir *redir)
{
    if (!redir)
        return;

    free_list_redir(redir->next);

    xfree(redir->ionumber);
    free_token_vect(redir->word);
    xfree(redir);
}

static void free_list_case_item(struct list_case_item *items)
{
    if (!items)
        return;

    free_list_case_item(items->next);

    free_token_vect(items->seq);
    handle_rec(items->statement, H_FREE);
}

static struct list_var_assign *dup_list_assign(struct list_var_assign *assign)
{
    if (!assign)
        return NULL;

    struct list_var_assign *res = xcalloc(1, sizeof(struct list_var_assign));

    res->next = dup_list_assign(assign->next);

    res->name = xstrdup(assign->name);

    res->value = dup_token_vect(assign->value);

    return res;
}

static struct list_redir *dup_list_redir(struct list_redir *redir)
{
    if (!redir)
        return NULL;

    struct list_redir *res = xcalloc(1, sizeof(struct list_redir));

    res->next = dup_list_redir(redir->next);

    res->ionumber = xstrdup(redir->ionumber);

    res->word = dup_token_vect(redir->word);

    return res;
}

static struct list_case_item *dup_list_case_item(struct list_case_item *items)
{
    if (!items)
        return NULL;

    struct list_case_item *res = xcalloc(1, sizeof(struct list_case_item));

    res->next = dup_list_case_item(items->next);

    res->seq = dup_token_vect(items->seq);

    res->statement = handle_rec(items->statement, H_DUP);

    return res;
}

void free_char_star_star(char **lst)
{
    int i = 0;
    while (lst[i])
    {
        xfree(lst[i]);
        i++;
    }

    xfree(lst);
}

char **dup_char_star_star(char **lst)
{
    int len = 0;
    int cap = 8;

    char **res = xcalloc(cap, sizeof(char *));

    while (lst[len])
    {
        if (len >= cap - 1)
        {
            cap *= 2;
            res = xrecalloc(res, cap * sizeof(char *));
        }

        res[len] = xstrdup(lst[len]);

        len++;
    }

    return res;
}

struct ast *handle_rec(struct ast *ast, enum handle h)
{
    if (!ast)
        return NULL;

    struct n_s_cmd *s_cmd_ast;
    struct n_binary *binary_ast;
    struct n_if *if_ast;
    struct n_command *cmd_ast;
    struct n_for *for_ast;
    struct n_func *func;
    struct n_case *case_ast;

    switch (ast->type)
    {
    case AST_S_CMD:
        s_cmd_ast = ast->t_ast;
        if (h == H_FREE)
        {
            free_token_vect(s_cmd_ast->cmd_arg);
            free_list_assign(s_cmd_ast->vars);
            xfree(s_cmd_ast);
            return NULL;
        }
        else
        {
            return build_s_cmd(dup_token_vect(s_cmd_ast->cmd_arg),
                               dup_list_assign(s_cmd_ast->vars));
        }
    case AST_IF:
        if_ast = ast->t_ast;
        if (h == H_FREE)
        {
            handle_rec(if_ast->condition, h);
            handle_rec(if_ast->true_c, h);
            handle_rec(if_ast->false_c, h);
            xfree(if_ast);
            return NULL;
        }
        else
        {
            return build_if(handle_rec(if_ast->condition, h),
                            handle_rec(if_ast->true_c, h),
                            handle_rec(if_ast->false_c, h));
        }
    case AST_WHILE:
    case AST_UNTIL:
    case AST_PIPE:
    case AST_LIST:
    case AST_AND:
    case AST_OR:
        binary_ast = ast->t_ast;
        if (h == H_FREE)
        {
            handle_rec(binary_ast->left, h);
            handle_rec(binary_ast->right, h);
            xfree(binary_ast);
            return NULL;
        }
        else
        {
            return build_binary(ast->type, handle_rec(binary_ast->left, h),
                                handle_rec(binary_ast->right, h));
        }
    case AST_FOR:
        for_ast = ast->t_ast;
        if (h == H_FREE)
        {
            xfree(for_ast->name);
            free_token_vect(for_ast->seq);
            handle_rec(for_ast->statement, h);
            xfree(for_ast);
            return NULL;
        }
        else
        {
            return build_for(xstrdup(for_ast->name),
                             dup_token_vect(for_ast->seq),
                             handle_rec(for_ast->statement, h));
        }
    case AST_SUBSHELL:
    case AST_NOT:
        if (h == H_FREE)
        {
            xfree(ast);
            return NULL;
        }
        else
        {
            return build_single(handle_rec(ast->t_ast, h), ast->type);
        }
    case AST_FUNC:
        func = ast->t_ast;
        if (h == H_FREE)
        {
            xfree(func->name);
            handle_rec(func->ast, h);
            xfree(func);
            return NULL;
        }
        else
        {
            return build_func(handle_rec(func->ast, h), xstrdup(func->name));
        }
    case AST_CMD:
        cmd_ast = ast->t_ast;
        if (h == H_FREE)
        {
            handle_rec(cmd_ast->ast, h);
            free_list_redir(cmd_ast->redirs);
            xfree(cmd_ast);
            return NULL;
        }
        else
        {
            return build_cmd(handle_rec(cmd_ast->ast, h),
                             dup_list_redir(cmd_ast->redirs));
        }
    case AST_CASE:
        case_ast = ast->t_ast;
        if (h == H_FREE)
        {
            free_token_vect(case_ast->pattern);
            free_list_case_item(case_ast->case_items);
            xfree(case_ast);
            return NULL;
        }
        else
        {
            return build_case(dup_token_vect(case_ast->pattern),
                              dup_list_case_item(case_ast->case_items));
        }
    case AST_BRACKET:
        break;
    case AST_PARENTH:
        break;
    }

    return NULL;
}
