#include "read_script.h"

#include <err.h>
#include <stdio.h>

#include "xalloc.h"

char *read_script(char *filename)
{
    FILE *f = fopen(filename, "r");
    if (f == NULL)
    {
        xfree_all();
        err(127, "Failed to open file.");
    }
    char *res = xcalloc(BUFFER_SIZE + 1, 1);
    int i = 0;
    size_t r = 0;
    while ((r = fread(res + i, 1, BUFFER_SIZE, f)))
    {
        i += r;
        res = xrealloc(res, i + 1 + BUFFER_SIZE);
        res[i] = '\0';
    }
    fclose(f);
    return res;
}
