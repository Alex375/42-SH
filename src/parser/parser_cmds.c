#include <errno.h>
#include <stddef.h>

#include "xalloc.h"
#include "xparser.h"
#include "xstrdup.h"

struct ast *parse_command()
{
    void *ast = NULL;
    struct list_redir *redirs = NULL;

    struct token_info tok = GET_TOKEN CHECK_SEG_ERROR(tok.type == T_EOF)

        if (tok.type == T_IF || tok.type == T_O_PRTH || tok.type == T_O_BRKT
            || tok.type == T_FOR || tok.type == T_WHILE
            || tok.type == T_UNTIL) // T_CASE
    {
        ast = parse_shell_command();
        parse_redirs(&redirs);
    }
    else
    {
        ast = parse_simple_command(&redirs);
    }

    if (!ast || errno != 0)
        return NULL;

    return build_cmd(ast, redirs);
}

static int is_chev(enum token tokT)
{
    if (tokT >= T_REDIR_1 && tokT <= T_REDIR_PIPE)
        return 1;

    return 0;
}

static int err_redir()
{
    enum token t0 = get_next_token().type;
    enum token t1 = look_forward_token(1).type;
    enum token t2 = look_forward_token(2).type;
    if ((is_chev(t0) && t1 != T_WORD)
        || (t0 == T_IONUMBER && ((!is_chev(t1)) || t2 != T_WORD)))
    {
        errno = ERROR_PARSING;
        return 1;
    }

    return 0;
}

static int is_redir()
{
    enum token t0 = get_next_token().type;
    enum token t1 = look_forward_token(1).type;
    enum token t2 = look_forward_token(2).type;
    if ((is_chev(t0) && t1 == T_WORD)
        || (t0 == T_IONUMBER && is_chev(t1) && t2 == T_WORD))
        return 1;

    return 0;
}

void *parse_redirs(struct list_redir **redirs)
{
    while (1)
    {
        if (err_redir() || !is_redir())
            break;

        struct token_info tok = GET_TOKEN

            struct list_redir *new_redir =
                xcalloc(1, sizeof(struct list_redir));

        if (tok.type == T_IONUMBER)
        {
            POP_TOKEN
            new_redir->ionumber = xstrdup(tok.command);
        }
        else
        {
            if (tok.type == T_REDIR_I_1 || tok.type == T_REDIR_I_A
                || tok.type == T_REDIR_O_2)
                new_redir->ionumber = xstrdup("0");
            else
                new_redir->ionumber = xstrdup("1");
        }

        tok = POP_TOKEN new_redir->redir_type = tok.type;

        new_redir->word = init_tok_vect();

        if (!add_word_vect(new_redir->word))
        {
            errno = ERROR_PARSING;
            return NULL;
        }

        add_to_redir_list(redirs, new_redir);
    }

    return NULL;
}

struct ast *parse_simple_command(struct list_redir **redirs)
{
    struct list_var_assign *vars = NULL;
    vars = parse_var_assignement(redirs);
    if (errno)
        return NULL;

    struct tok_vect *cmd_arg = init_tok_vect();

    while (add_word_vect(cmd_arg))
    {
        parse_redirs(redirs);
        if (errno)
            return NULL;
    }

    return build_s_cmd(cmd_arg, vars);
}

struct list_var_assign *parse_var_assignement(struct list_redir **redirs)
{
    parse_redirs(redirs);
    if (errno)
        return NULL;

    struct list_var_assign *res = NULL;

    struct token_info tok;
    while ((tok = get_next_token()).type == T_VAR_INIT)
    {
        struct list_var_assign *new =
            xcalloc(1, sizeof(struct list_var_assign));

        new->name = xstrdup(tok.command);

        POP_TOKEN

        if (tok.is_space_after)
            new->value = NULL;
        else
        {
            new->value = init_tok_vect();
            add_word_vect(new->value);
        }

        parse_redirs(redirs);
        if (errno)
            return NULL;
        add_to_var_assign_list(&res, new);
    }

    return res;
}
