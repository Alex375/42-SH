#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <xstrdup.h>

#include "xalloc.h"
#include "vars.h"
#include "xstring.h"

/*
 * Set_arr_PWD
 * Permet de creer un tableau contenant les differents dossiers parcouru pour
 * arriver au path actuel.
 *
 * EXEMPLE :
 *  -> `pwd` = /home/afs/ING1/42SH
 *  -> `getenv("PWD")` = /home/afs/ING1/42SH
 *  -> arr_PWD = [["HOME"], ["AFS"], ["ING1"], ["42SH"]]
 */
static struct string **set_arr_PWD(void)
{
    char *PWD = getenv("PWD");
    struct string **arr_PWD = NULL;
    int size = 0;
    char *token = strtok(PWD, "/");
    while (token != NULL)
    {
        arr_PWD = xrealloc(arr_PWD, ++size * sizeof(struct string *));
        arr_PWD[size - 1] = string_create();
        arr_PWD[size - 1] = string_concat(arr_PWD[size - 1], token);
        token = strtok(NULL, "/");
    }

    arr_PWD = xrealloc(arr_PWD, (size + 1) * sizeof(struct string *));
    arr_PWD[size] = NULL;
    return arr_PWD;
}

/*
 * Set_arr_PATH
 * Permet de creer un tableau contenant les differents dossiers a parcourir pour
 * atteindre le dossier cible vise par cd.
 *
 * EXEMPLE :
 *  -> `cd /home/afs/ING1/42SH`
 *  -> path = "/home/afs/ING1/42SH"
 *  -> arr_PATH = [["HOME"], ["AFS"], ["ING1"], ["42SH"]]
 */
static struct string **set_arr_PATH(char *path)
{
    struct string **arr_PATH = NULL;
    int size = 0;
    char *token = strtok(path, "/");
    while (token != NULL)
    {
        arr_PATH = xrealloc(arr_PATH, ++size * sizeof(struct string *));
        arr_PATH[size - 1] = string_create();
        arr_PATH[size - 1] = string_concat(arr_PATH[size - 1], token);
        token = strtok(NULL, "/");
    }

    arr_PATH = xrealloc(arr_PATH, (size + 1) * sizeof(struct string *));
    arr_PATH[size] = NULL;
    return arr_PATH;
}

/*
 * arr_append
 * Permet d'ajouter une chaine de charactere en derniere element d'un tableau
 *
 * EXEMPLE :
 *  -> arr = [["HELLO"], ["WORLD!"]]
 *  -> data = "BYE!"
 *
 *  Apres execution de arr_append(arr, data) :
 *   -> arr = [["HELLO"], ["WORLD!"], ["BYE!"]]
 */
static struct string **arr_append(struct string **arr, char *data)
{
    int size_arr = 0;
    for (int i = 0; arr[i] != NULL; i++)
        size_arr++;

    arr = xrealloc(arr, (size_arr + 1) * sizeof(struct string *));
    arr[size_arr] = string_create();
    arr[size_arr] = string_concat(arr[size_arr], data);
    return arr;
}

/*
 * arr_pop
 * Permet d'enlever une chaine de charactere en derniere element d'un tableau
 *
 * EXEMPLE :
 *  -> arr = [["HELLO"], ["WORLD!"], ["BYE!"]]
 *
 *  Apres execution de arr_append(arr) :
 *   -> arr = [["HELLO"], ["WORLD!"]]
 */
static struct string **arr_pop(struct string **arr)
{
    int size_arr = 0;
    for (int i = 0; arr[i] != NULL; i++)
        size_arr++;

    string_free(arr[size_arr - 1]);
    arr[size_arr - 1] = NULL;
    arr = xrealloc(arr, --size_arr * sizeof(struct string *));
    return arr;
}

/*
 *
 */
void set_Envar(char *path)
{
    char *old = xstrdup(getenv("PWD"));

    if (path[0] == '/') // si le path commence par un slash alors on reset $PWD
        setenv("PWD", "", 1);

    if (strcmp(path, getenv("HOME")) == 0)
    {
        setenv("OLDPWD", old, 1);
        add_var("OLDPWD", old);
        setenv("PWD", getenv("HOME"), 1);
        return;
    }
    struct string **arr_PWD = set_arr_PWD();
    struct string **arr_PATH = set_arr_PATH(path);

    for (int i = 0; arr_PATH[i] != NULL; i++)
    {
        if (strcmp(arr_PATH[i]->data, "..") == 0)
        {
            arr_PWD = arr_pop(arr_PWD);
        }

        else if (strcmp(arr_PATH[i]->data, ".") == 0)
            continue;

        else
            arr_PWD = arr_append(arr_PWD, arr_PATH[i]->data);
    }

    struct string *value_PWD = string_create();
    if (arr_PATH[0] == NULL)
        value_PWD = string_append(value_PWD, '/');

    for (int i = 0; arr_PWD[i] != NULL; i++)
    {
        value_PWD = string_append(value_PWD, '/');
        value_PWD = string_concat(value_PWD, arr_PWD[i]->data);
    }

    value_PWD = string_append(value_PWD, '\0');

    setenv("OLDPWD", old, 1);
    add_var("OLDPWD", old);
    setenv("PWD", value_PWD->data, 1);
}

int comeback(void)
{
    char *oldpwd;
    if ((oldpwd = getenv("OLDPWD")) == NULL)
    {
        fprintf(stderr, "Cannot obtain $OLDPWD");
        return 2;
    }

    chdir(oldpwd);
    char *tmp = xstrdup(getenv("PWD"));
    setenv("PWD", oldpwd, 1);
    setenv("OLDPWD", tmp, 1);
    add_var("OLDPWD", tmp);
    return 0;
}

int cd(char **args)
{
    char *path;
    if (args[1] == NULL)
        if (!getenv("HOME"))
            path = "/";
        else
            path = xstrdup(getenv("HOME"));

    else if (strcmp(args[1], "-") == 0)
    {
        int oui = comeback();
        return oui;
    }

    else
        path = xstrdup(args[1]); // on recup le path passe en arg


    int code_cd = chdir(path);
    if (code_cd == -1)
    {
        fprintf(stderr, "%s: No such file or directory\n", path);
        return 2;
    }

    //printf("before PWD : %s\n", getenv("PWD"));
    set_Envar(path);

    //printf("PWD : %s\n", getenv("PWD"));
    //printf("OLDPWD : %s\n", getenv("OLDPWD"));

    return 0;
}
