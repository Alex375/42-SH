#include <stdio.h>

#include "builtins.h"
#include "eval_ast.h"
#include "execution.h"
#include "options.h"
#include "special_vars.h"
#include "vars.h"
#include "xalloc.h"

int exec_func(char **args, struct ast *fc_ast)
{
    struct vars_vect *save = save_arg_var();
    int argc = 0;
    while (args[argc])
        argc++;

    set_arg_related_vars(argc, args);

    int res = eval_ast(fc_ast);

    for (int i = 0; i < save->len; ++i)
    {
        add_var(save->vars[i].name, save->vars[i].value);
    }
    for (int i = 0; i < save->argc - 1; ++i)
    {
        set_var_at(save->at[i], i);
    }
    for (int i = save->argc; i < get_vars_argc(); ++i)
    {
        set_var_at(NULL, i);

        char *str = xcalloc(16, sizeof(char));
        sprintf(str, "%d", i);
        add_var(str, "");
        xfree(str);
    }
    free_vars(save);

    return res;
}
