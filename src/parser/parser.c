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

struct ast *start_parse(char *script, size_t size)
{
    lexer_start(script, size);

    struct ast *res = parse_input();

    lexer_reset();

    return res;
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
    if (check_ender_token())
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
        struct token_info tok = GET_TOKEN

            if (!is_redir()) break;

        struct list_redir *new_redir = xcalloc(1, sizeof(struct list_redir));

        if (tok.type == T_WORD)
        {
            POP_TOKEN
            new_redir->ionumber = tok.command;
        }

        tok = POP_TOKEN new_redir->redir_type = tok.type;

        tok = POP_TOKEN CHECK_SEG_ERROR(tok.type != T_WORD) new_redir->word =
            tok.command;

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
            xrecalloc(cmd_arg, cap);
        }
        POP_TOKEN
        cmd_arg[i] = xstrdup(tok.command);
        i++;
    }

    return build_s_cmd(cmd, cmd_arg);
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
        return parse_if_rule(0);
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

struct ast *parse_compound()
{
    skip_newlines();

    if (check_ender_token())
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
