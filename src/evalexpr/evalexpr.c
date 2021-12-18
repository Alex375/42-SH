#include "evalexpr.h"

#include <err.h>

#include "xalloc.h"

static struct node *build_type(enum node_type t, int v, struct node *left,
                               struct node *right)
{
    struct node *ret = xmalloc(sizeof(struct node));
    if (t == NUMBER)
    {
        ret->type = NUMBER;
        ret->value = v;
    }
    else
        ret->type = t;

    ret->left = left;
    ret->right = right;
    return ret;
}

static struct node *get_nb(char *line, int strt, int end)
{
    char *nb = xmalloc(end - strt + 2);
    for (int j = strt; j <= end; j++)
    {
        nb[j - strt] = line[j];
    }
    nb[end - strt + 1] = '\0';
    int res = atoi(nb);
    xfree(nb);

    return build_type(NUMBER, res, NULL, NULL);
}

int validchar(char c, char c2)
{
    if (c == c2)
        return 1;
    if (c == ' ')
        return 1;
    int i = 0;
    while (i <= 9)
    {
        if (c == i + '0')
            return 1;
        i++;
    }

    return 0;
}

static int isBetweenParanthese(char *line, int strt, int end)
{
    if (line[strt] != '(')
        return 0;
    int open = 1;
    for (int j = strt + 1; j <= end; j++)
    {
        if (line[j] == '(')
            open++;
        if (line[j] == ')')
            open--;
        if (j != end && open <= 0)
            return 0;
    }
    return line[end] == ')';
}

static int skipParanth(char *line, int end, int inc, int j)
{
    char *p = "()";
    if (inc < 0)
        p = ")(";
    if (line[j] != p[0])
        return j;
    j += inc;
    int open = 1;
    while (j != end + inc)
    {
        if (line[j] == p[0])
            open++;
        if (line[j] == p[1])
            open--;
        if (open == 0)
            return j + inc;
        j += inc;
    }
    return j;
}

struct node *build_r(char *line, int strt, int end)
{
    if (end < strt)
        my_exit(2);
    int i = -1;
    int for1 = end;
    int for2 = strt;
    int inc = -1;
    while (++i < 9)
    {
        if (i == 5)
        {
            for1 = strt;
            for2 = end;
            inc = 1;
        }
        if (i == 8 && isBetweenParanthese(line, strt, end))
            return build_r(line, strt + 1, end - 1);
        for (int j = for1; j != for2 + inc; j += inc)
        {
            j = skipParanth(line, for2, inc, j);
            if (j == for2 + inc)
                break;
            if (line[j] != operators[i]
                || (i < 2 && (j == strt || !validchar(line[j - 1], ')'))))
                continue;
            struct node *left = (i <= 5) ? build_r(line, strt, j - 1) : NULL;
            struct node *right = build_r(line, j + 1, end);
            return build_type(i, 0, left, right);
        }
    }

    return get_nb(line, strt, end);
}

void expr_delete(struct node *root)
{
    if (root == NULL)
        return;
    expr_delete(root->left);
    expr_delete(root->right);
    xfree(root);
}

void my_exit(int code)
{
    xfree_all();
    err(code, "evalexpr failed");
}
