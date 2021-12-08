#include "special_vars.h"

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

#include "string.h"
#include "vars.h"
#include "xalloc.h"

void set_special_vars(int argc, char **argv)
{
    set_arg_related_vars(argc, argv);

    add_var("?", "0");

    set_var_int("$", geteuid());

    add_var("RANDOM", "0");

    set_var_int("UID", getpid());

    add_var("OLDPWD", getenv("OLDPWD"));

    add_var("IFS", " \t\n");
}

void set_arg_related_vars(int argc, char **argv)
{
    set_vars_argc(argc);

    set_var_int("#", argc - 1);

    struct string *star = string_create();
    for (int i = 1; i < argc; ++i)
    {
        char *str = xcalloc(16, sizeof(char));
        sprintf(str, "%d", i);
        add_var(str, argv[i]);
        xfree(str);

        set_var_at(argv[i], i - 1);

        star = string_concat(star, argv[i]);
        if (i < argc - 1)
            star = string_concat(star, " ");
    }

    add_var("*", string_get(star));
}
