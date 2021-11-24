#include "lexer.h"

int look_ahead_keywords(const char *script, size_t size)
{
    size_t i = g_lexer_info.pos;

    if (i > size)
        return 1;

    struct string *accumulator = string_create();

    accumulator = string_append(accumulator, script[i]);
    if (separatorify(accumulator->data) != -1)
    {
        string_free(accumulator);
        return 0;
    }

    i++;
    if (i > size)
        return 0;

    accumulator = string_append(accumulator, script[i]);
    if (separatorify(accumulator->data) != -1)
    {
        string_free(accumulator);
        return 0;
    }

    string_free(accumulator);
    return 1;
}

struct token_info lex_keywords(struct token_info res, struct string *string)
{
    g_lexer_info.context = GENERAL;
    string_free(string);

    return res;
}
