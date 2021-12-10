#define _POSIX_C_SOURCE 200112L


#include <err.h>
#include <stdlib.h>
#include <string.h>

#include "xalloc.h"
#include "xstrdup.h"
#if __APPLE__
#    include "xfnmatch.h"
#else
#    define _GNU_SOURCE
#    include <fnmatch.h>
#endif

int export(char **args)
{
    int i = 1;
    while (args[i] && args[i][0])
    {
        if (fnmatch("+([a-zA-Z0-9_])=*", args[i], FNM_EXTMATCH) != 0)
            errx(2, "export: '%s': bad variable name", args[i]);

        char *copy = xstrdup(args[i]);
        char *pos_equal = strchr(copy, '=');
        if (pos_equal == NULL)
            errx(2, "export: '%s': bad variable name", args[i]);

        *pos_equal = '\0';
        char *var_name = copy;
        char *var_value = "";

        if (*(pos_equal + 1) != '\0')
            var_value = pos_equal + 1;

        if (setenv(var_name, var_value, 1) == -1)
            errx(2, "export: the maximum has been reached");

        i++;
        xfree(copy);
    }

    return 0;
}
