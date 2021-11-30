#ifndef INC_42_SH_STRING_H
#define INC_42_SH_STRING_H

#include <stddef.h>

struct string
{
    size_t capacity;
    size_t size;
    char *data;
};

/**
** @brief                   Init a string.
*/
struct string *string_create();

/**
** @brief                   Append a char to a string.
** @param string            String source.
** @param c                 Char that will be append.
*/
struct string *string_append(struct string *string, char c);

/**
** @brief                   Concatenate a char* to a string.
** @param destination       String destination.
** @param source            Char* that will be concatenate.
*/
struct string *string_concat(struct string *destination, char *source);

struct string *string_pop(struct string *string, char *pop);
/**
** @brief                   Free the data structure around string and return the
*string date (you will need to free it).
** @param destination       String data that will be free.
*/
void *string_get(struct string *string);

/**
** @brief                   Free a string structure.
** @param destination       String structure that will be free.
*/
void string_free(struct string *string);

#endif // INC_42_SH_STRING_H
