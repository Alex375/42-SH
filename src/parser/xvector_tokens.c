#include <errno.h>

#include "ast_xalloc.h"
#include "handle_ast.h"
#include "vector_tokens.h"
#include "xparser.h"
#include "xstrdup.h"

int is_part_word(enum token t)
{
    return t == T_WORD || t == T_VAR || t == T_VAR_INQUOTE
        || t == T_COMMAND_SUB_START || t == T_COMMAND_SUB_START_Q
        || t == T_BACKQUOTE_START || t == T_BACKQUOTE_START_Q
        || t == T_EVALEXPR;
}

struct tok_vect *init_tok_vect()
{
    struct tok_vect *res = xcalloc(1, sizeof(struct tok_vect));

    res->cap = 8;
    res->list = xcalloc(res->cap, sizeof(struct token_info));
    res->cmd_sub_list = xcalloc(res->cap, sizeof(struct ast *));

    return res;
}

int add_word_vect(struct tok_vect *tok_vect, int quote_word)
{
    int added = 0;

    struct token_info tok;
    while (is_part_word((tok = get_next_token()).type))
    {
        added = 1;

        if (tok_vect->len >= tok_vect->cap - 1)
        {
            tok_vect->cap *= 2;
            tok_vect->list = xrecalloc(
                tok_vect->list, tok_vect->cap * sizeof(struct token_info));
            tok_vect->cmd_sub_list = xrecalloc(
                tok_vect->cmd_sub_list, tok_vect->cap * sizeof(struct ast *));
        }
        pop_token();

        if (tok.type == T_COMMAND_SUB_START || tok.type == T_COMMAND_SUB_START_Q
            || tok.type == T_BACKQUOTE_START || tok.type == T_BACKQUOTE_START_Q)
        {
            tok_vect->cmd_sub_list[tok_vect->len] = parse_compound();
            struct token_info after_sub = pop_token();

            if (((tok.type == T_COMMAND_SUB_START
                  || tok.type == T_COMMAND_SUB_START_Q)
                 && after_sub.type != T_COMMAND_SUB_END)
                || ((tok.type == T_BACKQUOTE_START
                     || tok.type == T_BACKQUOTE_START_Q)
                    && after_sub.type != T_BACKQUOTE_END))
            {
                errno = ERROR_PARSING;
                return 0;
            }
            tok_vect->list[tok_vect->len] = tok;
            if (tok.type == T_COMMAND_SUB_START
                || tok.type == T_BACKQUOTE_START)
                tok_vect->list[tok_vect->len].type = T_COMMAND_SUB_START;
            else
                tok_vect->list[tok_vect->len].type = T_COMMAND_SUB_START_Q;

            tok_vect->list[tok_vect->len].is_space_after =
                after_sub.is_space_after;
        }
        else
            tok_vect->list[tok_vect->len] = tok;

        if (quote_word)
        {
            if (tok_vect->list[tok_vect->len].type == T_VAR)
                tok_vect->list[tok_vect->len].type = T_VAR_INQUOTE;
            if (tok_vect->list[tok_vect->len].type == T_COMMAND_SUB_START)
                tok_vect->list[tok_vect->len].type = T_COMMAND_SUB_START_Q;
        }

        tok_vect->len++;

        if (tok.is_space_after)
            return 1;
    }

    if (added)
    {
        tok_vect->list[tok_vect->len - 1].is_space_after = 1;
    }
    return added;
}

struct tok_vect *dup_token_vect(struct tok_vect *vect)
{
    if (!vect)
        return NULL;

    struct tok_vect *res = xcalloc(1, sizeof(struct tok_vect));

    res->cap = vect->cap;
    res->len = vect->len;
    res->list = xcalloc(res->cap, sizeof(struct token_info));
    res->cmd_sub_list = xcalloc(res->cap, sizeof(struct ast *));

    for (int i = 0; i < res->len; ++i)
    {
        res->list[i] = vect->list[i];
        res->cmd_sub_list[i] = vect->cmd_sub_list[i];
    }

    return res;
}

void free_token_vect(struct tok_vect *vect)
{
    if (!vect)
        return;
    xfree(vect->list);
    for (int i = 0; i < vect->len; ++i)
    {
        handle_rec(vect->cmd_sub_list[i], H_FREE);
    }
    xfree(vect);
}
