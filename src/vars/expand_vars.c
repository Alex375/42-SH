#include <string.h>

#include "vars.h"
#include "xalloc.h"
#include "xparser.h"
#include "xstrdup.h"
#include "xstring.h"

char *get_word(struct tok_vect *tok_vect, int *i, int *nb_strtok)
{
    struct string *res = string_create();
    int empty = 0;
    do
    {
        if ((empty = 0) || tok_vect->list[*i].type == T_WORD)
            string_concat(res, tok_vect->list[*i].command);
        else
        {
            char *var_value = get_var(tok_vect->list[*i].command);
            if ((!var_value || !var_value[0]) && (empty = 1))
            {
                continue;
            }

            if (tok_vect->list[*i].type == T_VAR_INQUOTE)
                string_concat(res, var_value);
            else
            {
                char *w = strtok(xstrdup(var_value), " \t\n\r");

                for (int j = 0; j < *nb_strtok; ++j)
                {
                    w = strtok(NULL, " \t\n\r");
                }
                if (w)
                {
                    string_concat(res, w);
                    (*nb_strtok)++;
                    if (strtok(NULL, " \t\n\r"))
                        break;
                }
                (*nb_strtok) = 0;
            }
        }
    } while (++(*i) < tok_vect->len
             && (empty || tok_vect->list[*i - 1].is_space_after == 0));

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

    if (tok_vect->len == 0)
    {
        res[0] = xcalloc(1, sizeof(char));
    }

    int i = 0;
    int nb_strtok = 0;
    while (i < tok_vect->len)
    {
        if (len >= cap - 1)
        {
            cap *= 2;
            res = xrecalloc(res, cap * sizeof(char *));
        }

        res[len++] = get_word(tok_vect, &i, &nb_strtok);
    }

    return res;
}
