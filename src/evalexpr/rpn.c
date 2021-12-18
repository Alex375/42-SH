#include "evalexpr.h"
#include "xalloc.h"

int isOperator(char c)
{
    for (int i = 0; operators[i]; i++)
        if (c == operators[i])
            return 1;
    return 0;
}

int is_num(char *line, int place, int len)
{
    int i = 0;
    while (place + i < len)
        if (!isdigit(line[place + i++]))
            return 0;
    return 1;
}

static int calc(char ope, int r1, int r2)
{
    switch (ope)
    {
    case '+':
        return r2 + r1;
    case '-':
        return r2 - r1;
    case '*':
        return r2 * r1;
    case '/':
        if (r1 == 0)
            exit(3);
        return r2 / r1;
    case '%':
        if (r1 == 0)
            exit(3);
        return r2 % r1;
    case '^':
        return myPow(r2, r1);
    default:
        exit(2);
    }
}

int my_strtok(char *line, int i, int *len)
{
    int j = i;
    while (!isspace(line[j]))
        if (!line[j++])
            return -1;
    while (isspace(line[j]))
        j++;
    if (!line[j])
        return -1;
    *len = 0;
    while (line[j + *len] && !isspace(line[j + *len]))
        (*len)++;
    return j;
}

static int my_atoi(char *line, int place, int len)
{
    char *nb = xmalloc(len + 1);
    for (int j = place; j < place + len; j++)
    {
        nb[j - place] = line[j];
    }
    nb[len] = '\0';
    char *end_ptr;
    int res = strtol(nb, &end_ptr, 10);
    xfree(nb);
    return res;
}

int rpn_rec(char *line, int *place, int len)
{
    if (isOperator(line[*place]) && len == 1)
    {
        int ope = *place;
        int nlen = 0;
        *place = my_strtok(line, *place, &nlen);
        if (*place == -1)
            exit(2);
        int r1 = rpn_rec(line, place, nlen);
        *place = my_strtok(line, *place, &nlen);
        if (*place == -1)
            my_exit(2);
        int r2 = rpn_rec(line, place, nlen);
        return calc(line[ope], r1, r2);
    }
    else if (is_num(line, *place, len))
    {
        return my_atoi(line, *place, len);
    }
    else
        exit(2);
}

int eval_rpn(char *line, int len)
{
    char *nline = xcalloc(len + 2, 1);
    int j = 0;
    for (int i = len - 1; i >= 0; i--)
        nline[i] = line[j++];
    nline[len] = '\0';
    line = nline;

    int nlen = 0;

    int place = 0;
    while (isspace(line[place]))
        place++;
    while (line[place] && !isspace(line[place + nlen]))
        nlen++;
    int res = rpn_rec(line, &place, nlen);
    if (my_strtok(line, place, &nlen) != -1)
        my_exit(2);

    return res;
}
