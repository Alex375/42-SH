#include <err.h>
#include <xstrdup.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "cd_vector.h"
#include "vars.h"

static struct vector *set_vector_PATH(char *path)
{
    struct vector *vec = vector_create();
    char *token = strtok(path, "/");
    vec->str = string_concat(vec->str, token);
    while (token)
    {
        vector_add(vec, token);
        token = strtok(NULL, "/");
    }

    return vec;
}

static struct vector *set_vector_PWD()
{
    char *pwd = getenv("PWD");
    struct vector *vec = vector_create();
    char *token = strtok(pwd, "/");
    while (token)
    {
        vector_add(vec, token);
        token = strtok(NULL, "/");
    }

    return vec;
}

static int set_var(char *path)
{
    char *old = xstrdup(getenv("PWD"));
    if (path[0] == '/')
        setenv("PWD", "", 1);

    struct vector *vector_PATH = set_vector_PATH(path);
    struct vector *vector_PWD = set_vector_PWD();

    for (struct vector *ptr = vector_PATH; ptr != NULL; ptr = ptr->next)
    {
        if (strcmp(ptr->str->data, "..") == 0)
            vector_pop(vector_PWD);

        else if (strcmp(ptr->str->data, ".") == 0)
            continue;

        else
            vector_add(vector_PWD, vector_PATH->str->data);
    }

    struct string *value_PWD = string_create();
    if (vector_PATH == NULL)
        value_PWD = string_append(value_PWD,'/');

    for (struct vector *ptr = vector_PWD; ptr != NULL; ptr = ptr->next)
    {
        value_PWD = string_append(value_PWD, '/');
        value_PWD = string_concat(value_PWD, vector_PWD->str->data);
    }

    value_PWD = string_append(value_PWD, '\0');
    setenv("OLDPWD", old, 1);
    add_var("OLDPWD", old);
    setenv("PWD", value_PWD->data, 1);

    return 0;
}

static int minus()
{
    char *current = getenv("PWD");
    char *old = getenv("OLDPWD");

    int code = chdir(old);
    if (code == -1)
        errx(2, "Cannot obtain $OLDPWD");

    char *tmp = xstrdup(current);
    setenv("PWD", old, 1);
    setenv("OLDPWD", tmp, 1);
    add_var("OLDPWD", tmp);

    free(tmp);
    free(current);
    free(old);

    return 0;
}

int cd_helpme(char **args)
{
    char *path;
    if (args[1] == NULL)
        if (!getenv("HOME"))
            path = "/";
        else
            path = xstrdup(getenv("HOME"));

    else if (strcmp(args[1], "-") == 0)
        return minus();

    else
        path = xstrdup(args[1]);

    int code = chdir(path);
    if (code == -1)
        errx(2, "%s: No such file or directory\n", path);

    set_var(path);
    return 0;

}
