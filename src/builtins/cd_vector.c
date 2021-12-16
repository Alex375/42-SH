#include "cd_vector.h"

#include <err.h>
#include <stdlib.h>

#include "xalloc.h"
#include "xstring.h"

struct vector *vector_create()
{
    struct vector *vec = xmalloc(sizeof(struct vector *));
    if (vec == NULL)
        return NULL;

    vec->str = string_create();
    vec->next = NULL;

    return vec;
}

struct vector *vector_init(struct vector *vec, struct vector *next, char *data)
{
    vec->str = string_concat(vec->str, data);
    vec->next = next;
    return vec;
}

void vector_add(struct vector *vec, char *data)
{
    struct vector *new = vector_create();
    if (vec == NULL || new == NULL)
        errx(2, "Cannot create a new vector in vector add (cd)");

    new = vector_init(new, NULL, data);

    struct vector *ptr = vec;
    while (ptr->next != NULL)
        ptr = ptr->next;

    ptr->next = new;
}

void vector_pop(struct vector *vec)
{
    if (vec != NULL)
    {
        struct vector *ptr = vec;
        while (ptr->next->next != NULL)
            ptr++;

        string_free(ptr->next->str);
        free(ptr->next);
        ptr->next = NULL;
    }
}
