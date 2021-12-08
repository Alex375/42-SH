#include <err.h>

#include "ast_xalloc.h"
#include "vars.h"

int unset(char **args)
{
    int i = 1;
    while (args[i])
    {
        if (args[i][0] == '\0')
        {
            i++;
            continue;
        }
        if (args[i][0] < '0' || args[i][0] > '9')
            add_var(args[i], "");
        else
        {
            xfree_all();
            err(2, "Unset: Bad variable name");
        }
        i++;
    }
    return 0;
}
