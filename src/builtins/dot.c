#include <err.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "read_script.h"
#include "xparser.h"
#include "xalloc.h"

int contain_slash(char *str)
{
    return strchr(str, '/') != NULL;
}

char *create_path(char *path, char *file)
{
    char *res = xcalloc(strlen(path) + strlen(file) + 2, sizeof(char));
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
    int contain_s = contain_slash(file);
    if (!contain_s)
    {
        char *folder = strtok(getenv("PATH"), ":");
        char *file_env = NULL;
        while (folder != NULL)
        {
            file_env = create_path(folder, file);
            if (access(file_env, F_OK) == 0)
            {
                char *script = read_script(file_env);
                return exec_script(script, strlen(file), 0);
            }
            folder = strtok(NULL, ":");
        }
    }

    if (access(file, F_OK) != 0)
    {
        int error_code = contain_s ? 127 : 2;
        errx(error_code, "Can't open %s", file);
    }

    char *script = read_script(file);
    return exec_script(script, strlen(file), 0);
}
