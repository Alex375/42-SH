#include "xstrdup.h"

#include <string.h>

#include "xalloc.h"

char *xstrdup(const char *s)
{
    char *new_str = xcalloc(strlen(s) + 1, sizeof(char));
    return strcpy(new_str, s);
}
