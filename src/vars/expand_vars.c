#include "vars.h"
#include "xalloc.h"
#include "xstrdup.h"
#include "xstring.h"

static int is_in(const char *arr, char c)
{
    int i = 0;
    while (arr[i])
    {
        if (arr[i] == c)
            return 1;
        i++;
    }

    return 0;
}

static char *my_strsep(char *copy, char *ifs, int nb_strtok)
{
    const char *whitespace = " \t\n";

    char *w = NULL;
    for (int j = 0; j < nb_strtok + 1; ++j)
    {
        if (!copy || !*copy)
            return NULL;
        w = copy;
        int skipped_non_wp = 0;

        while (*copy)
        {
            if (is_in(ifs, *copy))
            {
                if (!is_in(whitespace, *copy))
                    skipped_non_wp = 1;
                *copy = '\0';
                copy++;
                break;
            }
            copy++;
        }

        while (*copy && is_in(ifs, *copy))
        {
            if (!is_in(whitespace, *copy))
            {
                if (!skipped_non_wp)
                    skipped_non_wp = 1;
                else
                    break;
            }

            copy++;
        }
    }

    return w;
}

static char *get_word(struct tok_vect *tok_vect, int *i, int *nb_strtok,
                      int *nb_at)
{
    struct string *res = string_create();
    int ept = 0;
    do
    {
        if ((ept = 0) || tok_vect->list[*i].type == T_WORD)
            string_concat(res, tok_vect->list[*i].command);
        else
        {
            char *val = get_var(tok_vect->list[*i].command, nb_at);
            if ((!val || !val[0]) && (ept = 1))
            {
                continue;
            }

            if (tok_vect->list[*i].type == T_VAR_INQUOTE)
                string_concat(res, val);
            else
            {
                char *w =
                    my_strsep(xstrdup(val), get_var("IFS", 0), *nb_strtok);

                if (w)
                {
                    string_concat(res, w);
                    (*nb_strtok)++;
                    if (my_strsep(xstrdup(val), get_var("IFS", 0), *nb_strtok))
                    {
                        if (*nb_at)
                            (*nb_at)--;
                        break;
                    }
                }
                (*nb_strtok) = 0;
            }
        }
    } while (!*nb_at && ++(*i) < tok_vect->len
             && (ept || tok_vect->list[*i - 1].is_space_after == 0));

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
    int nb_at = 0;
    while (i < tok_vect->len)
    {
        if (len >= cap - 1)
        {
            cap *= 2;
            res = xrecalloc(res, cap * sizeof(char *));
        }

        res[len++] = get_word(tok_vect, &i, &nb_strtok, &nb_at);
    }

    return res;
}
