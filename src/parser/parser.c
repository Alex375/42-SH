#include "headers/parser.h"

#include <errno.h>
#include <stddef.h>

#include "xalloc.h"

struct ast *parse_input(struct lexer_list **lex)
{
    return parse_list(lex);
}

struct ast *parse_list(struct lexer_list **lex)
{
    struct ast *left;
    struct ast *right;

    left = parse_and_or(lex);

    if (!left || errno == ERROR_PARSING
        || ((*lex)->type == T_NEWLINE || (*lex)->type == T_EOF))
        return left;

    if ((*lex)->type != T_SEMICOLON)
        return NULL;

    // POP
    *lex = (*lex)->next;
    right = parse_list(lex);

    if (!right || errno == ERROR_PARSING || (*lex)->type != T_SEMICOLON)
    {
        if ((*lex)->type != T_SEMICOLON)
            errno = ERROR_PARSING;
        return NULL;
    }

    // BUILD AND/OR
    struct n_binary *nBinary = xcalloc(1, sizeof(struct n_binary));
    nBinary->left = left;
    nBinary->right = right;

    struct ast *res = xcalloc(1, sizeof(struct ast));
    res->type = AST_LIST;
    res->t_ast = nBinary;

    return res;
}

struct ast *parse_and_or(struct lexer_list **lex)
{
    if (!lex || !*lex) // maybe not
    {
        errno = ERROR_PARSING;
        return NULL;
    }

    struct ast *left;
    struct ast *right;

    left = parse_pipeline(lex);

    if (!left || errno == ERROR_PARSING
        || ((*lex)->type != T_OR && (*lex)->type != T_AND))
        return left;

    // POP
    *lex = (*lex)->next;
    right = parse_and_or(lex);

    if (!right || errno == ERROR_PARSING)
        return NULL;

    // BUILD AND/OR
    struct n_binary *nBinary = xcalloc(1, sizeof(struct n_binary));
    nBinary->left = left;
    nBinary->right = right;

    struct ast *res = xcalloc(1, sizeof(struct ast));
    res->type = AST_IF;
    res->t_ast = nBinary;

    return res;
}

struct ast *parse_pipeline(struct lexer_list **lex)
{
    if (!lex || !*lex) // maybe not
    {
        errno = ERROR_PARSING;
        return NULL;
    }

    void *ast;
    enum token tok = (*lex)->type;

    if (tok == T_NOT)
    {
        // POP
        *lex = (*lex)->next;
        ast = parse_pipeline(lex);
    }
    else
    {
        struct ast *left = parse_command(lex);
        if (!left || errno == ERROR_PARSING || (*lex)->type != T_PIPE)
            return left;

        // POP
        *lex = (*lex)->next;
        struct ast *right = parse_pipeline(lex);

        if (!right || errno == ERROR_PARSING)
            return NULL;

        // BUILD AND/OR
        struct n_binary *nBinary = xcalloc(1, sizeof(struct n_binary));
        nBinary->left = left;
        nBinary->right = right;

        ast = nBinary;
    }
    
    if (!ast || errno == ERROR_PARSING)
        return NULL;

    struct ast *res = xcalloc(1, sizeof(struct ast));
    res->type = (tok == T_NOT) ? AST_NOT : AST_PIPE;
    res->t_ast = ast;

    return res;
}

struct ast *parse_command(struct lexer_list **lex)
{
    if (!lex || !*lex) // maybe not
    {
        errno = ERROR_PARSING;
        return NULL;
    }

    if ((*lex)->type == T_IF) // || for || while ...
        return parse_shell_command(lex);

    return parse_simple_command(lex);
}

struct ast *parse_simple_command(struct lexer_list **lex)
{
    if (!lex || !*lex) // maybe not
    {
        errno = ERROR_PARSING;
        return NULL;
    }

    if ((*lex)->type != T_COMMAND)
        return NULL;

    // POP
    struct lexer_list *elt = *lex;
    *lex = (*lex)->next;

    // BUILD CMD
    struct n_cmd *nCmd = xcalloc(1, sizeof(struct n_cmd));
    nCmd->cmd_line = elt->command;

    struct ast *res = xcalloc(1, sizeof(struct ast));
    res->type = AST_NOT;
    res->t_ast = nCmd;

    return res;
}

struct ast *parse_shell_command(struct lexer_list **lex)
{
    if (!lex || !*lex) // maybe not
    {
        errno = ERROR_PARSING;
        return NULL;
    }

    enum token tok = (*lex)->type;
    // POP
    *lex = (*lex)->next;

    struct ast *res;
    // switch
    switch (tok)
    {
    case T_O_BRKT:
    case T_O_PRTH:
        res = parse_compound(lex);
        if (errno == ERROR_PARSING
            || (tok == T_O_BRKT && (*lex)->type != T_C_BRKT)
            || (tok == T_O_PRTH && (*lex)->type != T_C_PRTH))
        {
            errno = ERROR_PARSING;
            return NULL;
        }
        // POP
        *lex = (*lex)->next;
        return res;
    case T_IF:
        return parse_if_rule(lex);

    default:
        return NULL;
    }
}

struct ast *parse_if_rule(struct lexer_list **lex)
{
    if (!lex || !*lex) // maybe not
    {
        errno = ERROR_PARSING;
        return NULL;
    }

    struct ast *condition;
    struct ast *true = NULL;
    struct ast *false = NULL;

    // POP
    *lex = (*lex)->next;
    condition = parse_compound(lex);

    if (!condition || errno == ERROR_PARSING || (*lex)->type != T_THEN)
        return NULL;

    // POP
    *lex = (*lex)->next;
    true = parse_compound(lex);

    if (!true || errno == ERROR_PARSING)
        return NULL;

    if ((*lex)->type == T_ELSE)
    {
        // POP
        *lex = (*lex)->next;
        false = parse_compound(lex);
        if (!false || errno == ERROR_PARSING)
            return NULL;
    }

    if ((*lex)->type != T_FI)
    {
        errno = ERROR_PARSING;
        return NULL;
    }
    // POP
    *lex = (*lex)->next;

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

struct ast *parse_compound(struct lexer_list **lex)
{
    struct ast *left;
    struct ast *right;

    left = parse_and_or(lex);

    if (!left || errno == ERROR_PARSING
        || ((*lex)->type == T_NEWLINE || (*lex)->type == T_EOF))
        return left;

    if ((*lex)->type != T_SEMICOLON)
        return NULL;

    // POP
    *lex = (*lex)->next;
    right = parse_list(lex);

    if (!right || errno == ERROR_PARSING
        || ((*lex)->type != T_SEMICOLON && (*lex)->type != T_NEWLINE))
    {
        if ((*lex)->type != T_SEMICOLON && (*lex)->type != T_NEWLINE)
            errno = ERROR_PARSING;
        return NULL;
    }

    // BUILD AND/OR
    struct n_binary *nBinary = xcalloc(1, sizeof(struct n_binary));
    nBinary->left = left;
    nBinary->right = right;

    struct ast *res = xcalloc(1, sizeof(struct ast));
    res->type = AST_LIST;
    res->t_ast = nBinary;

    return res;
}

void skip_newlines(struct lexer_list **lex)
{
    // while (see_tok()->type == T_NEWLINE)
    //   pop_tok();

    if (lex) // trash
        return;
}
