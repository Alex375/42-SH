#include "evalexpr.h"
#include "xalloc.h"

int launch_tree(char *line, int len)
{
    struct node *res = build_r(line, 0, len - 1);

    int r = expr_evaluate(res);

    expr_delete(res);

    return r;
}

static void check_clean(char *line, int len)
{
    for (int i = 0; i < len; i++)
    {
        char c = line[i];
        if (isspace(c) || isOperator(c) || isdigit(c))
            continue;
        my_exit(2);
    }
}

char *valid(char *line, int *len)
{
    char *new = xmalloc(16000);
    int newlen = 0;
    for (int i = 0; i < *len; i++)
    {
        char c = line[i];
        if (isspace(c))
            continue;
        if (isdigit(c) || isOperator(c) || c == '(' || c == ')')
            new[newlen++] = c;
        else
        {
            xfree(new);
            my_exit(1);
        }
    }
    new[newlen] = '\0';

    *len = newlen;
    return new;
}

void verifSpaces(char *line, int len)
{
    int i = 0;
    while (isspace(line[i]))
        i++;
    while (i < len)
    {
        while (i < len && !isspace(line[i]))
            i++;
        if (i == len)
            return;
        char n1 = line[i - 1];
        while (i < len && isspace(line[i]))
            i++;
        if (i == len)
            return;
        char n2 = line[i];
        if (n1 >= '0' && n1 <= '9' && n2 >= '0' && n2 <= '9')
            my_exit(2);
    }
}

char *fullCheck(char *line1, int *len)
{
    verifSpaces(line1, *len);
    char *line = valid(line1, len);
    int j = 0;
    int open = 0;
    while (j < *len)
    {
        if (line[j] == '(')
        {
            if (j > 0 && validchar(line[j - 1], ')'))
                exit(2);
            open++;
        }
        if (line[j] == ')')
        {
            if (j < *len - 1 && validchar(line[j - 1], '('))
                exit(2);
            open--;
        }
        if (open < 0)
            my_exit(2);
        j++;
    }
    if (open != 0)
        my_exit(2);

    return line;
}

char *launch_evalexpr(char *line)
{
    if (!line)
        my_exit(2);

    int len = strlen(line);

    check_clean(line, len);

    int r = 0;
    line = fullCheck(line, &len);
    if (len == 0)
        my_exit(2);
    r = launch_tree(line, len);

    char *str = xcalloc(16, sizeof(char));
    sprintf(str, "%d", r);

    return str;
}
