#include <err.h>
#include <stdlib.h>

#include "sys/types.h"
#include "unistd.h"
#include "vars.h"
#include "xalloc.h"

int xbreak(char **args)
{
    char *end_ptr;
    if (args[1] == NULL)
        return 256;
    long code = strtol(args[1], &end_ptr, 10);
    if (*end_ptr != '\0' || code <= 0)
    {
        xfree_all();
        err(2, "break: illegal number in\n");
    }
    return 255 + code;
}

int xcontinue(char **args)
{
    char *end_ptr;
    if (args[1] == NULL)
        return -1;
    long code = strtol(args[1], &end_ptr, 10);
    if (*end_ptr != '\0' || code <= 0)
    {
        xfree_all();
        err(2, "continue: illegal number in\n");
    }
    return -code;
}
