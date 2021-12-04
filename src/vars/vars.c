#include "vars.h"

#include <string.h>
#include <time.h>
#include <stdlib.h>
#include "stdio.h"


#include "xalloc.h"
#include "xparser.h"
#include "xstrdup.h"

extern struct vars_vect *vars;

struct vars_vect *init_vars_vect()
{
    struct vars_vect *res = xcalloc(1, sizeof(struct vars_vect));

    res->cap = 8;
    res->vars = xcalloc(res->cap, sizeof(struct var));

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

void set_var_int(char *name, long value)
{
    char *str = xcalloc(16, sizeof(char));
    sprintf(str,"%ld",value);
    add_var(name, str);
    xfree(str);
}

char *get_var(char *name)
{
    if (!strcmp("RANDOM", name))
    {
        srand(time(NULL));
        int r = rand() % 32768;
        char *str = xcalloc(16, sizeof(char));
        sprintf(str,"%d",r);
        return str;
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
