#include "special_vars.h"

#include "vars.h"
#include "xalloc.h"

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

void set_at_star_n(int argc, char **argv)
{
    for (int i = 0; i < argc; ++i)
    {
        char *str = xcalloc(16, sizeof(char));
        sprintf(str,"%d",i);
        add_var(str, argv[i]);
        xfree(str);
    }

    add_var("@", "");
    add_var("*", "");
}

void set_special_vars(int argc, char **argv)
{
    add_var("?", "0");

    set_var_int("$", geteuid());

    set_at_star_n(argc, argv);

    set_var_int("#", argc);

    add_var("RANDOM", "0");

    set_var_int("UID", getpid());

    add_var("OLDPWD", getenv("OLDPWD"));

    add_var("IFS", " \t\n");
}
