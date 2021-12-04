#include "special_vars.h"

#include "vars.h"
#include "xalloc.h"

#include <stdio.h>

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
    add_var("$", "0");

    set_at_star_n(argc, argv);

    char *str = xcalloc(16, sizeof(char));
    sprintf(str,"%d",argc);
    add_var("#", str);
    xfree(str);

    add_var("RANDOM", "0");
    add_var("UID", "0");
    add_var("OLDPWD", "0");
    add_var("IFS", " \t\n");

}
