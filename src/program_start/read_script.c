#include "read_script.h"

#include <err.h>
#include <stdio.h>
#include <stdlib.h>

char *read_script(char *filename, size_t *size)
{
    FILE *f = fopen(filename, "r");
    if (f == NULL)
    {
        // TODO properly exit program
        err(1, "No such a file  %s", filename);
    }
    char *res = malloc(BUFFER_SIZE + 1);
    *size = 0;
    size_t r = 0;
    while ((r = fread(res + *size, 1, BUFFER_SIZE + 1, f)))
    {
        *size += r;
        res = realloc(res, *size);
    }
    res[(*size)++] = '\0';
    return res;
}
