
#include "func.h"
#include "handle_ast.h"
#include "vars.h"
#include "xalloc.h"
#include "xparser.h"
#include "xstrdup.h"

#include <stdio.h>

extern struct context *context;

static char *get_var_var(struct vars_vect *vars, char *name, int *i_at)
{
    struct vars_vect *save = context->vars;
    context->vars = vars;
    char *r = get_var(name, i_at);

    context->vars = save;

    return r;
}

void push_pop_arg(struct vars_vect *var, int pop)
{
    if (!pop)
        save_arg_var(context->vars, var);
    else
        save_arg_var(var, context->vars);
}

void save_arg_var(struct vars_vect *src, struct vars_vect *dst)
{
    dst->argc = src->argc;

    add_var_var(dst, "#", get_var_var(src, "#", NULL));

    for (int i = 1; i < dst->argc; ++i)
    {
        char *str = xcalloc(16, sizeof(char));
        sprintf(str, "%d", i);
        add_var_var(dst, str, get_var_var(src, str, NULL));
        xfree(str);
    }

    add_var_var(dst, "*", get_var_var(src, "*", NULL));

    dst->at = dup_char_star_star(src->at);
}

void add_var_var(struct vars_vect *vars, char *name, char *value)
{
    struct vars_vect *save = context->vars;
    context->vars = vars;
    add_var(name, value);

    context->vars =save;
}

void set_vars_argc(int argc)
{
    context->vars->argc = argc;
}

int get_vars_argc()
{
    return context->vars->argc;
}
