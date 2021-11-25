#include <stddef.h>
#include <string.h>

#include "builtins.h"

int is_builins(char *function)
{
    char *builints[1] = { "echo" };
    for (int i = 0; i < 1; i++)
    {
        if (strcmp(builints[i], function) == 0)
            return i;
    }
    return -1;
}

int exec_builtin(int index, char *args)
{
    int (*builints[])(char *) = { is_builins };
    return builints[index](args);
}
