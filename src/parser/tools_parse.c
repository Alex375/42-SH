#include "headers/parser.h"
#include "xstrdup.h"

#include <errno.h>

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

static int is_chev(enum token tokT)
{
    if (tokT >= T_REDIR_1 && tokT <= T_REDIR_PIPE)
        return 1;

    return 0;
}

int err_redir()
{
    enum token t0 = get_next_token().type;
    enum token t1 = look_forward_token(1).type;
    enum token t2 = look_forward_token(2).type;
    if ((is_chev(t0) && t1 != T_WORD)
        || (is_chev(t1)
            && ((t0 != T_WORD && t0 != T_IONUMBER) || t2 != T_WORD)))
    {
        errno = ERROR_PARSING;
        return 1;
    }

    return 0;
}

int is_redir()
{
    enum token t0 = get_next_token().type;
    enum token t1 = look_forward_token(1).type;
    enum token t2 = look_forward_token(2).type;
    if ((is_chev(t0) && t1 == T_WORD)
        || (t0 == T_IONUMBER && is_chev(t1) && t2 == T_WORD))
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
