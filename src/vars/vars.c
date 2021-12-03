#include "vars.h"

#include <string.h>

#include "xalloc.h"
#include "xparser.h"
#include "xstrdup.h"

struct vars_vect *init_vars_vect()
{
    struct vars_vect *res = xcalloc(1, sizeof(struct vars_vect));

    res->cap = 8;
    res->vars = xcalloc(res->cap, sizeof(struct var));

    return res;
}

void add_var(struct vars_vect *vars, char *name, char *value)
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
        xrecalloc(vars->vars, vars->cap * sizeof(struct var));
    }

    vars->vars[vars->len].name = xstrdup(name);
    vars->vars[vars->len].name = xstrdup(value);

    vars->len++;
}
