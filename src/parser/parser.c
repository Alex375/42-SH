#include "headers/parser.h"

#include <errno.h>
#include <stddef.h>

#include "../xalloc/headers/xalloc.h"

struct ast *parse_input(struct lexer_list **lex)
{
    return parse_list(lex);
}

struct ast *parse_list(struct lexer_list **lex)
{
    return parse_and_or(lex);
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

    if (!left || errno == ERROR_PARSING || ((*lex)->type != T_OR && (*lex)->type != T_AND))
        return left;

    //POP
    *lex = (*lex)->next;
    right = parse_pipeline(lex);

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

    if ((*lex)->type != T_NOT)
        return parse_command(lex);

    struct ast *ast;

    //POP
    *lex = (*lex)->next;
    ast = parse_command(lex);

    if (!ast || errno == ERROR_PARSING)
        return NULL;

    // BUILD NOT
    struct n_not *nNot = xcalloc(1, sizeof(struct n_not));
    nNot->ast = ast;

    struct ast *res = xcalloc(1, sizeof(struct ast));
    res->type = AST_NOT;
    res->t_ast = nNot;

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

    //POP
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

    // switch
    switch ((*lex)->type)
    {
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

    if ((*lex)->type != T_IF)  // maybe unneeded
        return NULL;

    struct ast *condition;
    struct ast *true;
    struct ast *false;

    //POP
    *lex = (*lex)->next;
    condition = parse_compound(lex);

    if (!condition || errno == ERROR_PARSING || (*lex)->type != T_THEN)
        return NULL;

    //POP
    *lex = (*lex)->next;
    true = parse_compound(lex);

    if (!true || errno == ERROR_PARSING || (*lex)->type != T_ELSE)
        return NULL;

    //POP
    *lex = (*lex)->next;
    false = parse_compound(lex);

    if (!false || errno == ERROR_PARSING)
        return NULL;

    //BUILD IF
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
    return parse_and_or(lex);
}

