#include "headers/parser.h"

#include "xstrdup.h"

void skip_newlines()
{
    while (get_next_token().type == T_NEWLINE)
        pop_token();
}

int check_ender_token()
{
    struct token_info tok = get_next_token();

    if (tok.type == T_EOF || tok.type == T_THEN || tok.type == T_ELSE
        || tok.type == T_ELIF || tok.type == T_FI || tok.type == T_C_PRTH
        || tok.type == T_C_BRKT || tok.type == T_DO || tok.type == T_DONE)
        return 1;

    return 0;
}

static int is_chev(enum token tokT)
{
    if (tokT >= T_REDIR_1 && tokT <= T_REDIR_PIPE)
        return 1;

    return 0;
}

int is_redir()
{
    struct token_info tok = get_next_token();
    if ((is_chev(tok.type) && look_forward_token(1).type == T_WORD)
        || (tok.type == T_WORD && is_chev(look_forward_token(1).type)
            && look_forward_token(2).type == T_WORD))
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
