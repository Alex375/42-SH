#include <errno.h>
#include <stddef.h>

#include "xalloc.h"
#include "xparser.h"
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

struct ast *parse_shell_command()
{
    struct token_info tok =
        POP_TOKEN CHECK_SEG_ERROR(tok.type == T_EOF) enum token baseType =
            tok.type;

    struct ast *res;
    switch (baseType)
    {
    case T_O_BRKT:
    case T_O_PRTH:
        res = parse_compound();

        tok = POP_TOKEN CHECK_SEG_ERROR(
            errno == ERROR_PARSING || tok.type == T_EOF
            || (baseType == T_O_BRKT && tok.type != T_C_BRKT)
            || (baseType == T_O_PRTH && tok.type != T_C_PRTH))

            return res;
    case T_IF:
        return parse_if_rule(0);
    case T_FOR:
        return parse_for_rule();
    case T_WHILE:
    case T_UNTIL:
        return parse_while_until_rule(tok.type);
        // T_CASE

    default:
        errno = ERROR_PARSING;
        return NULL;
    }
}

struct ast *parse_if_rule(int inElif)
{
    struct ast *condition = NULL;
    struct ast *true_c = NULL;
    struct ast *false_c = NULL;

    condition = parse_compound();

    struct token_info tok = POP_TOKEN CHECK_SEG_ERROR(tok.type == T_EOF)

        if (!condition || errno != 0 || tok.type != T_THEN)
    {
        if (tok.type != T_THEN)
            errno = ERROR_PARSING;
        return NULL;
    }

    true_c = parse_compound();

    tok = GET_TOKEN CHECK_SEG_ERROR(tok.type == T_EOF)

        if (!true_c || errno != 0) return NULL;

    if (tok.type == T_ELSE)
    {
        POP_TOKEN
        false_c = parse_compound();
        if (!false_c || errno != 0)
            return NULL;
    }
    else if (tok.type == T_ELIF)
    {
        POP_TOKEN
        false_c = parse_if_rule(1);
        if (!false_c || errno != 0)
            return NULL;
    }

    tok = GET_TOKEN if (!inElif){ POP_TOKEN }

    CHECK_SEG_ERROR(tok.type != T_FI)

        return build_if(condition, true_c, false_c);
}

struct ast *parse_while_until_rule(enum token tokT)
{
    struct ast *condition = NULL;
    struct ast *statement = NULL;

    condition = parse_compound();

    struct token_info tok = POP_TOKEN CHECK_SEG_ERROR(tok.type == T_EOF)

        if (!condition || errno != 0 || tok.type != T_DO)
    {
        if (tok.type != T_DO)
            errno = ERROR_PARSING;
        return NULL;
    }

    statement = parse_compound();

    tok = POP_TOKEN CHECK_SEG_ERROR(tok.type != T_DONE)

        if (!statement || errno != 0) return NULL;

    return build_binary((tokT == T_WHILE) ? AST_WHILE : AST_UNTIL, condition,
                        statement);
}

struct ast *parse_for_rule()
{
    char *name = NULL;
    int cap = 8;
    char **seq = xcalloc(cap, sizeof(char *));
    struct ast *statement = NULL;

    struct token_info tok = POP_TOKEN CHECK_SEG_ERROR(tok.type != T_VAR)

        name = xstrdup(tok.command);

    skip_newlines();
    tok = GET_TOKEN CHECK_SEG_ERROR(tok.type != T_SEMICOLON && tok.type != T_IN
                                    && tok.type != T_DO)

        if (tok.type != T_DO)
    {
        POP_TOKEN;
    }
    if (tok.type == T_IN)
    {
        int i = 0;
        while ((tok = get_next_token()).type == T_WORD)
        {
            if (i >= cap - 1)
            {
                cap *= 2;
                seq = xrecalloc(seq, cap * sizeof(char *));
            }
            POP_TOKEN;
            seq[i] = xstrdup(tok.command);
            i++;
        }

        tok = POP_TOKEN CHECK_SEG_ERROR(tok.type != T_SEMICOLON
                                        && tok.type != T_NEWLINE)
    }

    skip_newlines();

    tok = POP_TOKEN CHECK_SEG_ERROR(tok.type == T_EOF)

        if (tok.type != T_DO)
    {
        errno = ERROR_PARSING;
        return NULL;
    }

    statement = parse_compound();

    tok = POP_TOKEN CHECK_SEG_ERROR(tok.type != T_DONE)

        if (!statement || errno != 0) return NULL;

    return build_for(name, seq, statement);
}
