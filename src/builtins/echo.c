#include <err.h>
#include <stdio.h>
#include <string.h>

#include "xalloc.h"
#include "xstring.h"
#include "builtins.h"
#include "xstrdup.h"

struct string *echo_pars(char *s, struct string *new_s, int flag_e)
{
    int i = 0;
    while (s[i] != '\0')
    {
        if (flag_e && '\\' == s[i] && 'n' == s[i + 1])
        {
            new_s = string_append(new_s, '\n');
            i++;
        }
        else if (flag_e && '\\' == s[i] && s[i + 1] == 't')
        {
            new_s = string_append(new_s, '\t');
            i++;
        }

        else if (flag_e && '\\' == s[i] && s[i + 1] == '\\')
        {
            new_s = string_append(new_s, '\\');
            i++;
        }
        else
            new_s = string_append(new_s, s[i]);
        i++;
    }

    return new_s;
}

int parse_option(struct string *res, char *token, int *flag_e, int *flag_n)
{
    if (strcmp("-ne", token) == 0 || strcmp("-en", token) == 0)
    {
        *flag_e = 1;
        *flag_n = 1;
        return 1;
    }
    if (strcmp("-e", token) == 0)
    {
        *flag_e = 1;
        return 1;
    }
    if (strcmp("-n", token) == 0)
    {
        *flag_n = 1;
        return 1;
    }
    echo_pars(token, res, *flag_e);
    string_append(res, ' ');
    return 0;
}

void echo(char *args)
{
    struct string *new_s = string_create();
    char *temp_args = xstrdup(args);
    char *token = strtok(temp_args, " ");
    int arguments = 1;
    int flag_e = 0;
    int flag_n = 0;
    while (token != NULL)
    {
        if (arguments)
            arguments = parse_option(new_s, token, &flag_e, &flag_n);
        else
        {
            echo_pars(token, new_s, flag_e);
            string_append(new_s, ' ');
        }
        token = strtok(NULL, " ");
    }
    if (new_s->size > 0 && new_s->data[new_s->size - 1] == ' ')
        new_s->data[new_s->size - 1] = (flag_n) ? '\0' : '\n';
    printf("%s", new_s->data);
    fflush(stdout);
    string_free(new_s);
    xfree(temp_args);
}
