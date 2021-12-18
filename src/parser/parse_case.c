#include <errno.h>

#include "options.h"
#include "xalloc.h"
#include "xparser.h"
#include "xstrdup.h"

struct ast *parse_case_rule()
{
    struct tok_vect *pattern = init_tok_vect();

    if (!add_word_vect(pattern, 1))
    {
        errno = ERROR_PARSING;
        return NULL;
    }

    skip_newlines();

    struct token_info tok = POP_TOKEN CHECK_SEG_ERROR(tok.type != T_IN)

        skip_newlines();

    struct list_case_item *case_items = parse_case_clause();

    if (errno != 0)
        return NULL;

    tok = POP_TOKEN CHECK_SEG_ERROR(tok.type != T_ESAC)

        return build_case(pattern, case_items);
}

struct list_case_item *parse_case_clause()
{
    if (get_next_token().type == T_ESAC)
        return NULL;

    struct list_case_item *res = parse_case_item();

    struct token_info tok;
    if ((tok = get_next_token()).type == T_DOUBLE_SCOLON)
    {
        POP_TOKEN
        skip_newlines();
        res->next = parse_case_clause();
    }
    else
        skip_newlines();

    return res;
}

struct list_case_item *parse_case_item()
{
    struct list_case_item *res = xcalloc(1, sizeof(struct list_case_item));

    if (get_next_token().type == T_O_PRTH)
        pop_token();

    res->seq = init_tok_vect();

    add_word_vect(res->seq, 1);

    while (get_next_token().type == T_PIPE)
    {
        pop_token();
        add_word_vect(res->seq, 1);
    }

    struct token_info tok = POP_TOKEN CHECK_SEG_ERROR(tok.type != T_C_PRTH)

        skip_newlines();

    res->statement = parse_compound();

    if (errno != 0)
        return NULL;

    return res;
}
