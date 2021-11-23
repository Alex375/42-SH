#include "read_script.h"
#include "xalloc.h"

#include <err.h>
#include <stdio.h>

char *read_script(char *filename, size_t *size)
{
    FILE *f = fopen(filename, "r");
    if (f == NULL)
    {
        xfree_all();
        err(1, "No such a file  %s", filename);
    }
    char *res = xmalloc(BUFFER_SIZE + 1);
    *size = 0;
    size_t r = 0;
    while ((r = fread(res + *size, 1, BUFFER_SIZE, f)))
    {
        *size += r;
        res = xrealloc(res, *size + 1);
    }
    res[(*size)++] = '\0';
    fclose(f);
    return res;
}
