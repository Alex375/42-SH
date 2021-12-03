#include <errno.h>

#include "headers/xparser.h"
#include "xstrdup.h"

void skip_newlines()
{
    while (get_next_token().type == T_NEWLINE)
        pop_token();
}

int check_ender_token(int in_compound)
{
    struct token_info tok = get_next_token();

    if (tok.type == T_EOF || tok.type == T_THEN || tok.type == T_ELSE
        || tok.type == T_ELIF || tok.type == T_FI || tok.type == T_C_PRTH
        || tok.type == T_C_BRKT || tok.type == T_DO || tok.type == T_DONE
        || tok.type == T_IN || tok.type == T_OR || tok.type == T_AND
        || tok.type == T_SEMICOLON || tok.type == T_PIPE || tok.type == T_ERROR
        || (!in_compound && tok.type == T_NEWLINE))
        return 1;

    return 0;
}

void add_to_redir_list(struct list_redir **redirs, struct list_redir *new_redir)
{
    if (!*redirs)
    {
        *redirs = new_redir;
        return;
    }

    add_to_redir_list(&(*redirs)->next, new_redir);
}

void add_to_var_assign_list(struct list_var_assign **vars,
                            struct list_var_assign *new_var)
{
    if (!*vars)
    {
        *vars = new_var;
        return;
    }

    add_to_var_assign_list(&(*vars)->next, new_var);
}
