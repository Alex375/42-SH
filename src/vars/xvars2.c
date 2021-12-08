
#include "func.h"
#include "handle_ast.h"
#include "vars.h"
#include "xalloc.h"
#include "xparser.h"
#include "xstrdup.h"

extern struct context *context;

struct vars_vect *save_arg_var()
{
    struct vars_vect *res = init_vars_vect();

    for (int i = 0; i < context->vars->len; ++i)
    {
        add_var(context->vars->vars[i].name, context->vars->vars[i].value);
    }

    res->at = dup_char_star_star(context->vars->at);

    return res;
}

void set_vars_argc(int argc)
{
    context->vars->argc = argc;
}

int get_vars_argc()
{
    return context->vars->argc;
}
