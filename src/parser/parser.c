#include "headers/parser.h"

#include <errno.h>
#include <stddef.h>

#include "xalloc.h"

#define CHECK_SEG_ERROR(condition)                                             \
    if (condition)                                                             \
    {                                                                          \
        errno = ERROR_PARSING;                                                 \
        return NULL;                                                           \
    }

struct ast *parse_input(const char *script, size_t size)
{
    struct token_info tok = get_next_token(script, size);
    if (tok.type == T_EOF || tok.type == T_NEWLINE)
    {
        pop_token(script, size);
        if (tok.type == T_EOF)
            errno = ERROR_EMPTY_EOF;
        return NULL;
    }
    struct ast *ast = parse_list(script, size);

    tok = pop_token(script, size);
    CHECK_SEG_ERROR(tok.type != T_EOF && tok.type != T_NEWLINE)

    return ast;
}

struct ast *parse_list(const char *script, size_t size)
{
    struct ast *left;
    struct ast *right;

    left = parse_and_or(script, size);

    if (!left || errno != 0)
        return NULL;

    struct token_info tok = get_next_token(script, size);

    if (tok.type != T_SEMICOLON)
        return left;

    pop_token(script, size);

    right = parse_list(script, size);

    if (errno != 0)
        return NULL;

    if (!right)
        return left;

    // BUILD AND/OR
    struct n_binary *nBinary = xcalloc(1, sizeof(struct n_binary));
    nBinary->left = left;
    nBinary->right = right;

    struct ast *res = xcalloc(1, sizeof(struct ast));
    res->type = AST_LIST;
    res->t_ast = nBinary;

    return res;
}

struct ast *parse_and_or(const char *script, size_t size)
{
    struct ast *left;
    struct ast *right;

    left = parse_pipeline(script, size);

    struct token_info tok = get_next_token(script, size);

    if (!left || errno != 0
        || (tok.type != T_OR && tok.type != T_AND))
        return left;

    pop_token(script, size);

    skip_newlines(script, size);

    right = parse_and_or(script, size);

    if (!right || errno != 0)
        return NULL;

    // BUILD AND/OR
    struct n_binary *nBinary = xcalloc(1, sizeof(struct n_binary));
    nBinary->left = left;
    nBinary->right = right;

    struct ast *res = xcalloc(1, sizeof(struct ast));
    res->type = (tok.type == T_AND) ? AST_AND : AST_OR;
    res->t_ast = nBinary;

    return res;
}

struct ast *parse_pipeline(const char *script, size_t size)
{
    void *ast;

    struct token_info tok = get_next_token(script, size);
    CHECK_SEG_ERROR(tok.type == T_EOF)
    enum token baseType = tok.type;

    if (tok.type == T_NOT)
    {
        pop_token(script, size);
        ast = parse_pipeline(script, size);
    }
    else
    {
        struct ast *left = parse_command(script, size);

        tok = get_next_token(script, size);

        if (!left || errno != 0 || tok.type != T_PIPE)
            return left;

        pop_token(script, size);

        skip_newlines(script, size);

        struct ast *right = parse_pipeline(script, size);

        if (!right || errno != 0)
            return NULL;

        // BUILD AND/OR
        struct n_binary *nBinary = xcalloc(1, sizeof(struct n_binary));
        nBinary->left = left;
        nBinary->right = right;

        ast = nBinary;
    }

    if (!ast || errno != 0)
        return NULL;

    struct ast *res = xcalloc(1, sizeof(struct ast));
    res->type = (baseType == T_NOT) ? AST_NOT : AST_PIPE;
    res->t_ast = ast;

    return res;
}

struct ast *parse_command(const char *script, size_t size)
{
    struct token_info tok = get_next_token(script, size);
    CHECK_SEG_ERROR(tok.type == T_EOF)

    if (tok.type == T_IF || tok.type == T_O_PRTH
        || tok.type == T_O_BRKT) // || for || while ...
    {
        return parse_shell_command(script, size);
    }

    return parse_simple_command(script, size);
}

struct ast *parse_simple_command(const char *script, size_t size)
{
    struct token_info tok = pop_token(script, size);
    CHECK_SEG_ERROR(tok.type == T_EOF)

    if (tok.type != T_COMMAND)
    {
        errno = ERROR_PARSING;
        return NULL;
    }

    // BUILD CMD
    struct n_cmd *nCmd = xcalloc(1, sizeof(struct n_cmd));
    nCmd->cmd_line = tok.command;

    struct ast *res = xcalloc(1, sizeof(struct ast));
    res->type = AST_CMD;
    res->t_ast = nCmd;

    return res;
}

struct ast *parse_shell_command(const char *script, size_t size)
{
    struct token_info tok = pop_token(script, size);
    CHECK_SEG_ERROR(tok.type == T_EOF)
    enum token baseType = tok.type;

    struct ast *res;
    // switch
    switch (baseType)
    {
    case T_O_BRKT:
    case T_O_PRTH:
        res = parse_compound(script, size);

        tok = pop_token(script, size);
        CHECK_SEG_ERROR(errno != 0 || tok.type == T_EOF
                        || (baseType == T_O_BRKT && tok.type != T_C_BRKT)
                        || (baseType == T_O_PRTH && tok.type != T_C_PRTH))

        return res;
    case T_IF:
        return parse_if_rule(script, size, 0);

    default:
        errno = ERROR_PARSING;
        return NULL;
    }
}

struct ast *parse_if_rule(const char *script, size_t size, int inElif)
{
    struct ast *condition;
    struct ast *true = NULL;
    struct ast *false = NULL;

    condition = parse_compound(script, size);

    struct token_info tok = pop_token(script, size);
    CHECK_SEG_ERROR(tok.type == T_EOF)

    if (!condition || errno != 0 || tok.type != T_THEN)
    {
        if (tok.type != T_THEN)
            errno = ERROR_PARSING;
        return NULL;
    }

    true = parse_compound(script, size);

    tok = get_next_token(script, size);
    CHECK_SEG_ERROR(tok.type == T_EOF)

    if (!true || errno != 0)
        return NULL;

    if (tok.type == T_ELSE)
    {
        pop_token(script, size);
        false = parse_compound(script, size);
        if (!false || errno != 0)
            return NULL;
    }

    if (tok.type == T_ELIF)
    {
        pop_token(script, size);
        false = parse_if_rule(script, size, 1);
        if (!false || errno != 0)
            return NULL;
    }

    tok = get_next_token(script, size);
    if (!inElif)
        pop_token(script, size);

    CHECK_SEG_ERROR(tok.type != T_FI)

    // BUILD IF
    struct n_if *n_if = xcalloc(1, sizeof(struct n_if));
    n_if->condition = condition;
    n_if->true = true;
    n_if->false = false;

    struct ast *res = xcalloc(1, sizeof(struct ast));
    res->type = AST_IF;
    res->t_ast = n_if;

    return res;
}

struct ast *parse_compound(const char *script, size_t size)
{
    struct token_info tok = get_next_token(script, size);

    if (tok.type == T_EOF || tok.type == T_THEN || tok.type == T_ELSE
        || tok.type == T_FI || tok.type == T_C_PRTH || tok.type == T_C_BRKT)
        return NULL;

    struct ast *left;
    struct ast *right;

    skip_newlines(script, size);

    left = parse_and_or(script, size);

    tok = get_next_token(script, size);

    if (!left || errno != 0)
        return NULL;

    if (tok.type != T_SEMICOLON && tok.type != T_NEWLINE)
        return left;

    pop_token(script, size);

    skip_newlines(script, size);

    right = parse_compound(script, size);

    if (errno != 0)
        return NULL;

    if (!right)
        return left;

    // BUILD AND/OR
    struct n_binary *nBinary = xcalloc(1, sizeof(struct n_binary));
    nBinary->left = left;
    nBinary->right = right;

    struct ast *res = xcalloc(1, sizeof(struct ast));
    res->type = AST_LIST;
    res->t_ast = nBinary;

    return res;
}

void skip_newlines(const char *script, size_t size)
{
    while (get_next_token(script, size).type == T_NEWLINE)
        pop_token(script, size);
}
