#include "xstring.h"
#include "xalloc.h"

struct string *string_create()
{
    struct string *res = xcalloc(1, sizeof(struct string));
    res->capacity = 16;
    res->data = xcalloc(res->capacity, sizeof(char));
    return res;
}

struct string *string_append(struct string *string, char c)
{
    if (string->size == string->capacity)
    {
        string->capacity = (string->capacity - 1) * 2 + 1;
        string->data = xrecalloc(string->data, string->capacity * sizeof(char));
    }
    string->data[string->size] = c;
    string->size++;
    return string;
}

struct string *string_concat(struct string *destination, char *source)
{
    if (destination == NULL)
        return destination;
    size_t i = 0;
    while (source[i])
    {
        destination = string_append(destination, source[i]);
        i++;
    }
    return destination;
}

void *string_get(struct string *string)
{
    char *res = string->data;
    xfree(string);
    return res;
}

void string_free(struct string *string)
{
    xfree(string->data);
    xfree(string);
}