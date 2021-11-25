#include "headers/parser.h"

#include <errno.h>
#include <stddef.h>

#include "xalloc.h"
#include "xstrdup.h"
#include "xstring.h"


#define CHECK_SEG_ERROR(condition)                                             \
    if (condition)                                                             \
    {                                                                          \
        errno = ERROR_PARSING;                                                 \
        return NULL;                                                           \
    }

#define POP_TOKEN \
    pop_token();\
    if (tok.type == T_ERROR)\
    {                                                                          \
        errno = ERROR_PARSING;                                                 \
        return NULL;                                                           \
    }

#define GET_TOKEN \
    get_next_token();              \
    if (tok.type == T_ERROR)\
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
    struct token_info tok = GET_TOKEN
    if (tok.type == T_EOF || tok.type == T_NEWLINE)
    {
        POP_TOKEN
        if (tok.type == T_EOF)
            errno = ERROR_EMPTY_EOF;
        return NULL;
    }
    struct ast *ast = parse_list();

    tok = POP_TOKEN
    CHECK_SEG_ERROR(tok.type != T_EOF && tok.type != T_NEWLINE)

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

    if (tok.type != T_SEMICOLON)
        return left;

    POP_TOKEN

    right = parse_list();

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

struct ast *parse_and_or()
{
    struct ast *left = NULL;
    struct ast *right = NULL;

    left = parse_pipeline();

    struct token_info tok = GET_TOKEN

    if (!left || errno != 0 || (tok.type != T_OR && tok.type != T_AND))
        return left;

    POP_TOKEN

    skip_newlines();

    right = parse_and_or();

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

struct ast *parse_pipeline()
{
    void *ast = NULL;

    struct token_info tok = GET_TOKEN
    CHECK_SEG_ERROR(tok.type == T_EOF)
    enum token baseType = tok.type;

    if (tok.type == T_NOT)
    {
        POP_TOKEN
        ast = parse_pipeline();
    }
    else
    {
        struct ast *left = parse_command();

        tok = GET_TOKEN

        if (!left || errno != 0 || tok.type != T_PIPE)
            return left;

        POP_TOKEN

        skip_newlines();

        struct ast *right = parse_pipeline();

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

struct ast *parse_command()
{
    void *ast = NULL;
    struct list_redir *redirs = NULL;

    struct token_info tok = GET_TOKEN
    CHECK_SEG_ERROR(tok.type == T_EOF)

    if (tok.type == T_IF || tok.type == T_O_PRTH
        || tok.type == T_O_BRKT) // || for || while ...
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

    struct n_command *cmd = xcalloc(1, sizeof(struct n_command));
    cmd->ast = ast;
    cmd->redirs = redirs;

    return ast;
}

struct list_redir *parse_redirs()
{
    struct list_redir *res = NULL;

    while (1)
    {
        struct token_info tok = GET_TOKEN

        if (!is_redir())
            break;

        struct list_redir *new_redir = xcalloc(1, sizeof(struct list_redir));

        if (tok.type == T_WORD)
        {
            POP_TOKEN
            new_redir->ionumber = tok.command;
        }

        tok = POP_TOKEN
        new_redir->redir_type = tok.type;

        tok = POP_TOKEN
        CHECK_SEG_ERROR(tok.type != T_WORD)
        new_redir->word = tok.command;

        add_to_redir_list(&res, new_redir);
    }

    return res;
}

struct ast *parse_simple_command()
{
    struct token_info tok = POP_TOKEN
    CHECK_SEG_ERROR(tok.type == T_EOF)

    if (tok.type != T_WORD)
    {
        errno = ERROR_PARSING;
        return NULL;
    }

    // BUILD CMD
    struct n_s_cmd *nCmd = xcalloc(1, sizeof(struct n_s_cmd));
    nCmd->cmd = xstrdup(tok.command);
    nCmd->cmd_arg = string_create();

    while((tok = get_next_token()).type == T_WORD)
    {
        POP_TOKEN
        if (nCmd->cmd_arg->data[0])
            string_concat(nCmd->cmd_arg, " ");
        string_concat(nCmd->cmd_arg, tok.command);
    }

    struct ast *res = xcalloc(1, sizeof(struct ast));
    res->type = AST_S_CMD;
    res->t_ast = nCmd;

    return res;
}

struct ast *parse_shell_command()
{
    struct token_info tok = POP_TOKEN
    CHECK_SEG_ERROR(tok.type == T_EOF)
    enum token baseType = tok.type;

    struct ast *res;
    // switch
    switch (baseType)
    {
    case T_O_BRKT:
    case T_O_PRTH:
        res = parse_compound();

        tok = POP_TOKEN
        CHECK_SEG_ERROR(errno != 0 || tok.type == T_EOF
                        || (baseType == T_O_BRKT && tok.type != T_C_BRKT)
                        || (baseType == T_O_PRTH && tok.type != T_C_PRTH))

        return res;
    case T_IF:
        return parse_if_rule(0);

    default:
        errno = ERROR_PARSING;
        return NULL;
    }
}

struct ast *parse_if_rule(int inElif)
{
    struct ast *condition = NULL;
    struct ast *true = NULL;
    struct ast *false = NULL;

    condition = parse_compound();

    struct token_info tok = POP_TOKEN
    CHECK_SEG_ERROR(tok.type == T_EOF)

    if (!condition || errno != 0 || tok.type != T_THEN)
    {
        if (tok.type != T_THEN)
            errno = ERROR_PARSING;
        return NULL;
    }

    true = parse_compound();

    tok = GET_TOKEN
    CHECK_SEG_ERROR(tok.type == T_EOF)

    if (!true || errno != 0)
        return NULL;

    if (tok.type == T_ELSE)
    {
        POP_TOKEN
        false = parse_compound();
        if (!false || errno != 0)
            return NULL;
    }
    else if (tok.type == T_ELIF)
    {
        POP_TOKEN
        false = parse_if_rule(1);
        if (!false || errno != 0)
            return NULL;
    }

    tok = GET_TOKEN
    if (!inElif)
    {
        POP_TOKEN
    }

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

struct ast *parse_compound()
{
    if (check_ender_token())
        return NULL;

    struct ast *left = NULL;
    struct ast *right = NULL;

    skip_newlines();

    left = parse_and_or();

    struct token_info tok = GET_TOKEN

    if (!left || errno != 0)
        return NULL;

    if (tok.type != T_SEMICOLON && tok.type != T_NEWLINE)
        return left;

    POP_TOKEN

    skip_newlines();

    right = parse_compound();

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

void skip_newlines()
{
    while (get_next_token().type == T_NEWLINE)
        pop_token();
}

int check_ender_token()
{
    struct token_info tok = get_next_token();

    if (tok.type == T_EOF || tok.type == T_THEN || tok.type == T_ELSE
        || tok.type == T_FI || tok.type == T_C_PRTH || tok.type == T_C_BRKT)
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
