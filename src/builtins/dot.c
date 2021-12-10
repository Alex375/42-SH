#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <err.h>

#include "read_script.h"
#include "xparser.h"

int contain_slash(char *str)
{
    return strchr(str, '/') != NULL;
}

char *create_path(char *path, char* file)
{
    char* res = calloc(strlen(path) + strlen(file) + 2, sizeof(char));
    if (path[strlen(path) - 1] == '/')
    {
        sprintf(res, "%s%s", path, file);
    }
    else
    {
        sprintf(res, "%s/%s", path, file);
    }
    return res;
}

int dot(char **args)
{
    if (args[1] == NULL)
        return 0;

    char *file = args[1];
    if (!contain_slash(file))
    {
        char* folder = NULL;
        char* file_env = NULL;
        while ((folder = strtok(getenv("PATH"), ":")) != NULL)
        {
            file_env = create_path(folder, file);
            if(access(file_env, F_OK) == 0)
            {
                char *script = read_script(file_env);
                return exec_script(script, strlen(file));
            }
        }
    }

    if(access(file, F_OK) != 0)
    {
        errx(2, "export: the maximum has been reached");
    }

    char *script = read_script(file);
    return exec_script(script, strlen(file));;
}
