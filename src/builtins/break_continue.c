#include <err.h>
#include <stdlib.h>

#include "sys/types.h"
#include "unistd.h"
#include "vars.h"
#include "xalloc.h"
#include "ast_info.h"

static int break_or_continue(char **args, enum ast_info_type t)
{
    char *end_ptr;
    int code;
    if (args[1] == NULL)
        code = 1;
    else
    {
        code = strtol(args[1], &end_ptr, 10);
        if (*end_ptr != '\0' || code <= 0)
        {
            xfree_all();
            err(2, "break/continue: illegal number in\n");
        }
    }


    set_b_c(code, t);

    return 0;
}

int xbreak(char **args)
{
    return break_or_continue(args, A_BREAK);
}

int xcontinue(char **args)
{
    return break_or_continue(args, A_CONTINUE);
}
