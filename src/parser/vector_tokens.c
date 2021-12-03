#include <errno.h>
#include <stddef.h>

#include "xalloc.h"
#include "xparser.h"
#include "xstrdup.h"

static int is_part_word(enum token t)
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

int add_word_vect(struct tok_vect *tok_vect)
{
    int added = 0;

    struct token_info tok;
    while (is_part_word((tok = get_next_token()).type))
    {
        added = 1;

        if (tok_vect->len >= tok_vect->cap - 1)
        {
            tok_vect->cap *= 2;
            xrecalloc(tok_vect->list,
                      tok_vect->cap * sizeof(struct token_info));
        }
        pop_token();

        tok_vect->list[tok_vect->len] = tok;
        tok_vect->len++;

        if (tok.is_space_after)
            return 1;
    }

    return added;
}
