#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <xstrdup.h>

#include "vars.h"
#include "xalloc.h"

char *get_curdir(void)
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

int cd(char **args)
{
    char *path;
    char *oldpwd = getenv("PWD");
    if (args[1] == NULL)
    {
        if (!getenv("HOME"))
            path = "/";
        else
            path = xstrdup(getenv("HOME"));
        if (chdir(path) == -1)
        {
            fprintf(stderr, "%s: No such file or directory\n", path);
            return 2;
        }
        else
        {
            setenv("OLDPWD", oldpwd, 1);
            add_var("OLDPWD", oldpwd);
            setenv("PWD", path, 1);
        }
    }
    else if (strcmp(args[1], "-") == 0)
    {
        char *target = getenv("OLDPWD");
        if (chdir(target) == -1)
        {
            fprintf(stderr, "%s: No such file or directory\n", target);
            return 2;
        }
        else
        {
            printf("%s\n", target);
            setenv("OLDPWD", oldpwd, 1);
            add_var("OLDPWD", oldpwd);
            setenv("PWD", target, 1);
        }
    }
    else
    {
        path = xstrdup(args[1]);
        if (chdir(path) == -1)
        {
            fprintf(stderr, "%s: No such file or directory\n", path);
            return 2;
        }
        else
        {
            setenv("OLDPWD", oldpwd, 1);
            add_var("OLDPWD", oldpwd);
            char *curdir = get_curdir();
            setenv("PWD", curdir, 1);
            xfree(curdir);
        }
    }
    return 0;
}
