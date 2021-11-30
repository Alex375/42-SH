#include <errno.h>
#include <stddef.h>

#include "parser.h"
#include "xalloc.h"
#include "xstrdup.h"

#define CHECK_SEG_ERROR(condition)                                             \
    if (condition)                                                             \
    {                                                                          \
        errno = ERROR_PARSING;                                                 \
        return NULL;                                                           \
    }

#define POP_TOKEN                                                              \
    pop_token();                                                               \
    if (tok.type == T_ERROR)                                                   \
    {                                                                          \
        errno = ERROR_PARSING;                                                 \
        return NULL;                                                           \
    }

#define GET_TOKEN                                                              \
    get_next_token();                                                          \
    if (tok.type == T_ERROR)                                                   \
    {                                                                          \
        errno = ERROR_PARSING;                                                 \
        return NULL;                                                           \
    }

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
        || (t0 == T_IONUMBER
            && ((!is_chev(t1)) || t2 != T_WORD)))
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

        tok = POP_TOKEN CHECK_SEG_ERROR(tok.type != T_WORD) new_redir->word =
            xstrdup(tok.command);

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

    struct token_info tok;
    int cap = 8;
    int i = 0;
    char **cmd_arg = xcalloc(cap, sizeof(char *));
    while ((tok = get_next_token()).type == T_WORD)
    {
        if (i >= cap - 1)
        {
            cap *= 2;
            xrecalloc(cmd_arg, cap * sizeof(char *));
        }
        POP_TOKEN
        cmd_arg[i] = xstrdup(tok.command);

        parse_redirs(redirs);
        if (errno)
            return NULL;

        i++;
    }

    char *cmd = NULL;
    if (i > 0)
        cmd = xstrdup(cmd_arg[0]);

    return build_s_cmd(cmd, cmd_arg, vars);
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

        if ((tok = get_next_token()).type == T_VAR_VALUE)
        {
            new->value = xstrdup(tok.command);
            POP_TOKEN
        }
        else
            new->value = xcalloc(1, 1);

        parse_redirs(redirs);
        if (errno)
            return NULL;
        add_to_var_assign_list(&res, new);
    }

    return res;
}
