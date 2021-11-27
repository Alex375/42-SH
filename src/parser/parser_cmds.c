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
        redirs = parse_redirs();
    }
    else
    {
        ast = parse_simple_command();
    }

    if (!ast || errno != 0)
        return NULL;

    return build_cmd(ast, redirs);
}

struct list_redir *parse_redirs()
{
    struct list_redir *res = NULL;

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

        add_to_redir_list(&res, new_redir);
    }

    return res;
}

struct ast *parse_simple_command()
{
    struct token_info tok = POP_TOKEN CHECK_SEG_ERROR(tok.type == T_EOF)

        if (tok.type != T_WORD)
    {
        errno = ERROR_PARSING;
        return NULL;
    }

    char *cmd = xstrdup(tok.command);

    int cap = 8;
    int i = 1;
    char **cmd_arg = xcalloc(cap, sizeof(char *));
    cmd_arg[0] = cmd;
    while ((tok = get_next_token()).type == T_WORD)
    {
        if (i >= cap - 1)
        {
            cap *= 2;
            xrecalloc(cmd_arg, cap * sizeof(char *));
        }
        POP_TOKEN
        cmd_arg[i] = xstrdup(tok.command);
        i++;
    }

    return build_s_cmd(cmd, cmd_arg);
}
