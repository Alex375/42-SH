

#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "stdio.h"
#include "vars.h"
#include "xalloc.h"
#include "xparser.h"
#include "xstrdup.h"

extern struct context *context;

void add_var(char *name, char *value)
{
    int pos = 0;
    for (; pos < context->vars->len; ++pos)
    {
        if (!strcmp(context->vars->vars[pos].name, name))
            break;
    }

    if (pos != context->vars->len)
    {
        char *previous = context->vars->vars[pos].value;
        context->vars->vars[pos].value = xstrdup(value);
        xfree(previous);

        return;
    }

    if (context->vars->len >= context->vars->cap - 1)
    {
        context->vars->cap *= 2;
        context->vars->vars = xrecalloc(
            context->vars->vars, context->vars->cap * sizeof(struct var));
    }

    context->vars->vars[context->vars->len].name = xstrdup(name);
    context->vars->vars[context->vars->len].value = xstrdup(value);

    context->vars->len++;
}

void set_var_at(char *value, int i)
{
    context->vars->at = xrecalloc(context->vars->at, (i + 2) * sizeof(char *));
    if (value)
        context->vars->at[i] = xstrdup(value);
    else
        context->vars->at[i] = NULL;
}

void set_var_int(char *name, long value)
{
    char *str = xcalloc(16, sizeof(char));
    sprintf(str, "%ld", value);
    add_var(name, str);
    xfree(str);
}

char *get_var(char *name, int *i_at)
{
    if (!name)
        return xcalloc(1, 1);
    if (name && !strcmp("RANDOM", name))
    {
        srand(time(NULL));
        int r = rand() % 32768;
        char *str = xcalloc(16, sizeof(char));
        sprintf(str, "%d", r);
        return str;
    }

    if (name && !strcmp("@", name))
    {
        if (!context->vars->at[*i_at])
        {
            *i_at = 0;
            return "";
        }
        int old = *i_at;
        (*i_at)++;
        if (!context->vars->at[*i_at])
            *i_at = 0;
        return context->vars->at[old];
    }

    int pos = 0;
    for (; pos < context->vars->len; ++pos)
    {
        if (!strcmp(context->vars->vars[pos].name, name))
            break;
    }

    if (pos != context->vars->len)
        return context->vars->vars[pos].value;
    else
        return "";
}

int int_eq_var(char *var, int n)
{
    char *value = get_var(var, 0);
    if (value[0] == '\0')
        return 0;
    char *en_ptr;
    int int_value = strtol(value, &en_ptr, 10);
    if (*en_ptr != '\0')
        return 0;
    return int_value == n;
}
