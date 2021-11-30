#include <err.h>
#include <stdlib.h>

#include "xalloc.h"

int xexit(char **args)
{
    char *end_ptr;
    long exit_code = strtol(args[1], &end_ptr, 10);
    if (*end_ptr != '\0' || exit_code < 0)
    {
        xfree_all();
        err(2, "Illegal number in\n");
    }
    exit_code %= 256;
    xfree_all();
    exit(exit_code);
}
