#include <err.h>
#include <ctype.h>

#include "ast_xalloc.h"
#include "vars.h"

int export(char **args)
{
    int i = 1;
    int f = 0;
    while (args[i] && args[i][0] == '-' && args[i][1])
    {
        int j = 1;
        while (args[i][j] == 'v' || args[i][j] == 'f')
        {
            f = args[i][j++] == 'f';
        }
        if (args[i][j])
        {
            xfree_all();
            err(2, "unset: illegal option");
        }
        i++;
    }
    while (args[i])
    {
        if (args[i][0] == '\0')
        {
            i++;
            continue;
        }
        if (isalpha(args[i][0]) || args[i][0] == '_')
        {
            if (f)
            {
                destroy_fc(args[i]);
            }
            else
            {
                add_var(args[i], "");
                unsetenv(args[i]);
            }
        }
        else
        {
            xfree_all();
            err(2, "Unset: Bad variable name");
        }
        i++;
    }
    return 0;
}
