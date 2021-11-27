#include "parser.h"

#include <errno.h>
#include <stddef.h>

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

struct ast *parse_input()
{
    struct token_info tok =
        GET_TOKEN if (tok.type == T_EOF || tok.type == T_NEWLINE)
    {
        POP_TOKEN
        if (tok.type == T_EOF)
            errno = ERROR_EMPTY_EOF;
        return NULL;
    }
    struct ast *ast = parse_list();

    tok = POP_TOKEN CHECK_SEG_ERROR(tok.type != T_EOF && tok.type != T_NEWLINE)

        return ast;
}

struct ast *parse_list()
{
    if (check_ender_token(0))
        return NULL;

    struct ast *left = NULL;
    struct ast *right = NULL;

    left = parse_and_or();

    if (!left || errno != 0)
        return NULL;

    struct token_info tok = GET_TOKEN

        if (tok.type != T_SEMICOLON) return left;

    POP_TOKEN

    right = parse_list();

    if (errno != 0)
        return NULL;

    if (!right)
        return left;

    return build_binary(AST_LIST, left, right);
}

struct ast *parse_and_or()
{
    struct ast *left = NULL;
    struct ast *right = NULL;

    left = parse_pipeline();

    struct token_info tok = GET_TOKEN

        if (!left || errno != 0
            || (tok.type != T_OR && tok.type != T_AND)) return left;

    POP_TOKEN

    skip_newlines();

    right = parse_and_or();

    if (!right || errno != 0)
    {
        // free left
        if (!right)
            errno = ERROR_PARSING;
        return NULL;
    }

    return build_binary((tok.type == T_AND) ? AST_AND : AST_OR, left, right);
}

struct ast *parse_pipeline()
{
    void *ast = NULL;

    struct token_info tok =
        GET_TOKEN CHECK_SEG_ERROR(tok.type == T_EOF) enum token baseType =
            tok.type;

    if (tok.type == T_NOT)
    {
        POP_TOKEN
        ast = parse_pipeline();
    }
    else
    {
        struct ast *left = parse_command();

        tok = GET_TOKEN

            if (!left || errno != 0 || tok.type != T_PIPE) return left;

        POP_TOKEN

        skip_newlines();

        struct ast *right = parse_pipeline();

        if (!right || errno != 0)
        {
            // free left
            if (!right)
                errno = ERROR_PARSING;
            return NULL;
        }

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

struct ast *parse_compound()
{
    skip_newlines();

    if (check_ender_token(1))
        return NULL;

    struct ast *left = NULL;
    struct ast *right = NULL;

    left = parse_and_or();

    struct token_info tok = GET_TOKEN

        if (!left || errno != 0) return NULL;

    if (tok.type != T_SEMICOLON && tok.type != T_NEWLINE)
        return left;

    POP_TOKEN

    skip_newlines();

    right = parse_compound();

    if (errno != 0)
        return NULL;

    if (!right)
        return left;

    return build_binary(AST_LIST, left, right);
}
