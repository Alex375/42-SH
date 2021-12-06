#include <err.h>
#include <stdlib.h>

#include "xalloc.h"
#include "vars.h"
#include "sys/types.h"
#include "unistd.h"

int xexit(char **args)
{

    char *end_ptr;
    if (args[1] == NULL)
        exit(0);
    long exit_code = strtol(args[1], &end_ptr, 10);
    if (*end_ptr != '\0' || exit_code < 0)
    {
        xfree_all();
        err(2, "Illegal number in\n");
    }
    exit_code %= 256;
    pid_t pid = getpid();
    if (int_eq_var("$", pid))
        xfree_all();
    exit(exit_code);
}
