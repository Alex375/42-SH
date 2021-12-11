#include <stdio.h>
#include <stdlib.h>

#include "builtins.h"

int main()
{
    char *arg0 = "export";
    char *arg1 = "test=";
    char *arg2 = "test2=marche";
    char *arg3 = "test3=       a";
    char **args = calloc(5, sizeof(char *));
    args[0] = arg0;
    args[1] = arg1;
    args[2] = arg2;
    args[3] = arg3;

    int test = export(args);
    printf("%d\n", test);

    printf("TEST : %s\n", getenv("test"));
    printf("TEST2 : %s\n", getenv("test2"));
    printf("TEST3 : %s\n", getenv("test3"));

    free(args);

    return 0;
}
