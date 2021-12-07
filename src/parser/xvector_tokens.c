

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

    return added;
}
