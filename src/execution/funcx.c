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
    struct vars_vect *save = init_vars_vect();
    push_pop_arg(save, 0);
    int argc = 0;
    while (args[argc])
        argc++;

    set_arg_related_vars(argc, args);

    for (int i = get_vars_argc(); i <= save->argc; ++i)
    {
        set_var_at(NULL, i - 1);

        char *str = xcalloc(16, sizeof(char));
        sprintf(str, "%d", i);
        add_var(str, "");
        xfree(str);
    }

    int res = eval_ast(fc_ast);

    push_pop_arg(save, 1);
    for (int i = save->argc; i <= get_vars_argc(); ++i)
    {
        set_var_at(NULL, i - 1);

        char *str = xcalloc(16, sizeof(char));
        sprintf(str, "%d", i);
        add_var(str, "");
        xfree(str);
    }
    free_vars(save);

    return res;
}
