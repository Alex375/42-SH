#define _POSIX_C_SOURCE 200112L

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <vars.h>
#include <xalloc.h>

char *get_curdirr(void)
{
    char *res = xcalloc(1, 4097);
    int size = 4096;
    while (getcwd(res, 4096) == NULL)
    {
        res = xrecalloc(res, size * 2 + 1);
        size *= 2;
    }
    return res;
}

int pwd(char **args)
{
    if (!args[0])
        return 0;
    char *CWD = getenv("PWD");
    if (CWD == NULL)
        CWD = get_curdirr();
    printf("%s\n", CWD);
    return 0;
}
