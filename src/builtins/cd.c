#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <err.h>
#include <xstrdup.h>
#include <stdlib.h>

#include "xalloc.h"

/*
 Cette fonction permet de recuperer le path actuel, c'est a dire de recuperer
 le path avant modification de chdir().
 */
char *current_path_init(void)
{
    int current_size = 1024;
    char *current_path = xcalloc(1024, sizeof(char));
    while (getcwd(current_path, current_size) == NULL)
    {
        current_size += 1024;
        current_path = xrecalloc(current_path, current_size);
    }
    current_path = getcwd(current_path, current_size);

    return current_path;
}

int cd(char **args)
{
    char *path;
    if (args[1] == NULL)
        path = xstrdup(getenv("HOME")); // si pas d'arg

    else
        path = xstrdup(args[1]); // on recup le path passe en arg

    char *current_path = current_path_init();

    int code_cd = chdir(path);
    if (code_cd == -1)
    {
        fprintf(stderr, "%s: No such file or directory", path);
        return 2;
    }

    // On recup la len max pour cree notre derniere location (on on est apres
    // cd)
    int final_len = strlen(current_path) + strlen(path);
    char *final_path = xcalloc(final_len, sizeof(char));
    final_path = getcwd(final_path, final_len);
    if (strcmp(current_path, final_path) == 0)
        return 0;

    xfree(final_path);
    xfree(path);
    xfree(current_path);
    return 0;
}
