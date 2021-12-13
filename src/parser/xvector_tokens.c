

#include "ast_xalloc.h"
#include "vector_tokens.h"
#include "xstrdup.h"

int is_part_word(enum token t)
{
    return t == T_WORD || t == T_VAR || t == T_VAR_INQUOTE;
}

struct tok_vect *init_tok_vect()
{
    struct tok_vect *res = xcalloc(1, sizeof(struct tok_vect));

    res->cap = 8;
    res->list = xcalloc(res->cap, sizeof(struct token_info));

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
        }
        pop_token();

        tok_vect->list[tok_vect->len] = tok;
        if (quote_word && tok_vect->list[tok_vect->len].type == T_VAR)
            tok_vect->list[tok_vect->len].type = T_VAR_INQUOTE;
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
    struct tok_vect *res = xcalloc(1, sizeof(struct tok_vect));

    res->cap = vect->cap;
    res->len = vect->len;
    res->list = xcalloc(res->cap, sizeof(struct token_info));

    for (int i = 0; i < res->len; ++i)
    {
        res->list[i] = vect->list[i];
    }

    return res;
}

void free_token_vect(struct tok_vect *vect)
{
    if (!vect)
        return;
    xfree(vect->list);
    xfree(vect);
}
