#include <errno.h>
#include <stddef.h>

#include "xalloc.h"
#include "xparser.h"
#include "xstrdup.h"

static struct ast *O_BRKT()
{
    struct ast *res = parse_compound();

    struct token_info tok = POP_TOKEN CHECK_SEG_ERROR(
        errno == ERROR_PARSING || tok.type == T_EOF || tok.type != T_C_BRKT)

        return res;
}

static struct ast *O_PRTH()
{
    struct ast *ast = parse_compound();

    struct token_info tok = POP_TOKEN CHECK_SEG_ERROR(
        errno == ERROR_PARSING || tok.type == T_EOF || tok.type != T_C_PRTH)

        return build_single(ast, AST_SUBSHELL);
}

struct ast *parse_shell_command()
{
    struct token_info tok =
        POP_TOKEN CHECK_SEG_ERROR(tok.type == T_EOF) enum token baseType =
            tok.type;

    switch (baseType)
    {
    case T_O_BRKT:
        return O_BRKT();
    case T_O_PRTH:
        return O_PRTH();
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
    struct tok_vect *seq = NULL;
    struct ast *statement = NULL;

    struct token_info tok = POP_TOKEN CHECK_SEG_ERROR(tok.type != T_VAR)

        name = xstrdup(tok.command);

    skip_newlines();
    tok = GET_TOKEN CHECK_SEG_ERROR(tok.type != T_SEMICOLON && tok.type != T_IN
                                    && tok.type != T_DO)

        if (tok.type != T_DO)
    {
        POP_TOKEN
    }
    if (tok.type == T_IN)
    {
        seq = init_tok_vect();

        while (add_word_vect(seq, 0))
            ;

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
