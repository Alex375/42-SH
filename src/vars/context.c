

#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "stdio.h"
#include "vars.h"
#include "func.h"
#include "xalloc.h"
#include "xparser.h"
#include "xstrdup.h"
#include "handle_ast.h"

extern struct context *context;

struct context *init_context()
{
    context = xcalloc(1, sizeof(struct context));
    context->vars = init_vars_vect();
    context->fcs = init_func_vect();

    return context;
}

struct vars_vect *init_vars_vect()
{
    struct vars_vect *res = xcalloc(1, sizeof(struct vars_vect));

    res->cap = 8;
    res->vars = xcalloc(res->cap, sizeof(struct var));
    res->at = xcalloc(1, sizeof(char *));

    return res;
}

void free_vars()
{
    for (int i = 0; i < context->vars->len; ++i)
    {
        xfree(context->vars->vars[i].name);
        xfree(context->vars->vars[i].value);
    }

    xfree(context->vars->vars);

    free_char_star_star(context->vars->at);

    xfree(context->vars);
}

void free_context()
{
    free_vars();
    free_fcs();

    xfree(context);
}
