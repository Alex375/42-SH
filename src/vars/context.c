#include "func.h"
#include "handle_ast.h"
#include "vars.h"
#include "xalloc.h"
#include "xparser.h"
#include "xstrdup.h"

extern struct context *context;

struct vars_vect *init_vars_vect()
{
    struct vars_vect *res = xcalloc(1, sizeof(struct vars_vect));

    res->cap = 8;
    res->vars = xcalloc(res->cap, sizeof(struct var));
    res->at = xcalloc(1, sizeof(char *));

    return res;
}

struct context *init_context()
{
    context = xcalloc(1, sizeof(struct context));
    context->vars = init_vars_vect();
    context->fcs = init_func_vect();

    return context;
}

void free_vars(struct vars_vect *vars)
{
    for (int i = 0; i < vars->len; ++i)
    {
        xfree(vars->vars[i].name);
        xfree(vars->vars[i].value);
    }

    xfree(vars->vars);

    free_char_star_star(vars->at);

    xfree(vars);
}

void free_context()
{
    free_vars(context->vars);
    free_fcs();

    xfree(context);
}
