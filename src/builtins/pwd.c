#include <stdio.h>
#include <stdlib.h>
#include <vars.h>

int pwd(char **args)
{
    if (!args[0])
        return 0;
    char *CWD = getenv("PWD");
    printf("%s\n", CWD);
    return 0;
}
