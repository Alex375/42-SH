#include <err.h>
#include <stdio.h>
#include <string.h>

#include "xstring.h"
#include "xalloc.h"

struct string *echo_pars(char *s, struct string *new_s)
{
    int i = 0;
    while (s[i] != '\0')
    {
        if ('\\' == s[i] && 'n' == s[i + 1])
        {
            new_s = string_append(new_s, '\n');
            i++;
        }

        else if ('\\' == s[i] && s[i + 1] == 't')
            new_s = string_append(new_s, '\t');

        else if ('\\' == s[i] && s[i + 1] == '\\')
            new_s = string_append(new_s, '\\');

        else
            new_s = string_append(new_s, s[i]);

        i++;
    }

    return new_s;
}

// TODO : preparse needed
void echo(char *s, char *args)
{
    if (s == NULL)
    {
        xfree_all();
        err(1, "echo.c: String to echo is NULL");
    }

    struct string *new_s = string_create();
    new_s = echo_pars(s, new_s);

    int flag_n = 0; // si flag = 0 on add un \n a la fin de notre string (-n)
    int flag_e = 0; // si flag = 1 on execute notre input

    char *token = strtok(args, " ");
    while (token != NULL)
    {
        if (strcmp("-n", token) == 0)
            flag_n = 1;

        else if (strcmp("-e", token) == 0)
            flag_e = 1;

        token = strtok(NULL, " ");
    }

    if (flag_n == 0)
        new_s = string_append(new_s, '\n');

    if (flag_e == 1)
    {
        printf("%s", new_s->data);
        string_free(new_s);
        return;
    }

    printf("%s", s);
    if (flag_n == 0)
        printf("\n");
    string_free(new_s);
}
