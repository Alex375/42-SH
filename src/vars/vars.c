

#include <string.h>
#include <time.h>
#include <stdlib.h>
#include "stdio.h"
#include "vars.h"

#include "xalloc.h"
#include "xparser.h"
#include "xstrdup.h"

extern struct vars_vect *vars;

struct vars_vect *init_vars_vect()
{
    struct vars_vect *res = xcalloc(1, sizeof(struct vars_vect));

    res->cap = 8;
    res->vars = xcalloc(res->cap, sizeof(struct var));
    res->at = xcalloc(1, sizeof(char *));

    return res;
}

void add_var(char *name, char *value)
{
    int pos = 0;
    for (; pos < vars->len; ++pos)
    {
        if (!strcmp(vars->vars[pos].name, name))
            break;
    }

    if (pos != vars->len)
    {
        xfree(vars->vars[pos].value);
        vars->vars[pos].value = xstrdup(value);

        return;
    }

    if (vars->len >= vars->cap - 1)
    {
        vars->cap *= 2;
        vars->vars = xrecalloc(vars->vars, vars->cap * sizeof(struct var));
    }

    vars->vars[vars->len].name = xstrdup(name);
    vars->vars[vars->len].value = xstrdup(value);

    vars->len++;
}

void set_var_at(char *value, int i)
{
    vars->at = xrecalloc(vars->at, (i + 2) * sizeof(char *));
    vars->at[i] = xstrdup(value);
}

void set_var_int(char *name, long value)
{
    char *str = xcalloc(16, sizeof(char));
    sprintf(str,"%ld",value);
    add_var(name, str);
    xfree(str);
}

char *get_var(char *name, int *i_at)
{
    if (name && !strcmp("RANDOM", name))
    {
        srand(time(NULL));
        int r = rand() % 32768;
        char *str = xcalloc(16, sizeof(char));
        sprintf(str,"%d",r);
        return str;
    }

    if (name && !strcmp("@", name))
    {
        if (!vars->at[*i_at])
        {
            *i_at = 0;
            return "";
        }
        int old = *i_at;
        (*i_at)++;
        if (!vars->at[*i_at])
            *i_at = 0;
        return vars->at[old];
    }

    int pos = 0;
    for (; pos < vars->len; ++pos)
    {
        if (!strcmp(vars->vars[pos].name, name))
            break;
    }

    if (pos != vars->len)
        return vars->vars[pos].value;
    else
        return "";
}

int int_eq_var(char *var, int n)
{
    char *value = get_var(var, 0);
    if (value[0] == '\0')
        return 0;
    char *en_ptr;
    int int_value = strtol(value, &en_ptr, 10);
    if (*en_ptr != '\0')
        return 0;
    return int_value == n;
}
