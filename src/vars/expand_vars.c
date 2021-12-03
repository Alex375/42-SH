#include <string.h>

#include "vars.h"
#include "xalloc.h"
#include "xparser.h"
#include "xstrdup.h"
#include "xstring.h"

char *get_word(struct tok_vect *tok_vect, int *i,
               int *nb_strtok)
{
    struct string *res = string_create();
    do
    {
        if (tok_vect->list[*i].type == T_WORD)
            string_concat(res, tok_vect->list[*i].command);
        else if (tok_vect->list[*i].type == T_VAR_INQUOTE)
            string_concat(res, get_var(tok_vect->list[*i].command));
        else
        {
            char *copy = xstrdup(get_var(tok_vect->list[*i].command));
            char *w = NULL;
            for (int j = 0; j < *nb_strtok + 1; ++j)
            {
                w = strtok(copy, " \t\n\r");
            }
            if (!w)
                (*nb_strtok) = 0;
            else
            {
                string_concat(res, w);
                (*nb_strtok)++;
                w = strtok(copy, " \t\n\r");
                if (w)
                    break;
                else
                    (*nb_strtok) = 0;
            }
        }

        (*i)++;
    } while (*i < tok_vect->len && tok_vect->list[*i - 1].is_space_after == 0);

    return string_get(res);
}

char **expand_vars_vect(struct tok_vect *tok_vect)
{
    int cap = 8;
    int len = 0;
    char **res = xcalloc(cap, sizeof(char *));

    if (!tok_vect)
    {
        res[0] = xcalloc(1, sizeof(char *));
        return res;
    }

    int i = 0;
    int nb_strtok = 0;
    while (i < tok_vect->len)
    {
        if (len >= cap - 1)
        {
            cap *= 2;
            xrecalloc(res, cap * sizeof(char *));
        }

        res[len++] = get_word(tok_vect, &i, &nb_strtok);
    }

    return res;
}
