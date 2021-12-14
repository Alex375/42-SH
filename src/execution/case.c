#include "eval_ast.h"

#include <err.h>
#include <string.h>

#include "ast_info.h"
#include "execution.h"

int treat_case(struct n_case *case_ast)
{
    int res;

    char **pattern = expand_vars_vect(case_ast->pattern);

    struct list_case_item *item = case_ast->case_items;

    while (item != NULL)
    {
        char **seq = expand_vars_vect(item->seq);

        int i = 0;
        while (seq[i] && fnmatch(pattern[0], seq[i], 0) != 0)
            i++;

        if (seq[i])
        {
            EVAL_AST(item->statement);
            RETURN(res);
        }

        item = item->next;
    }

    RETURN(0);
}
