#include "lexer.h"

#include <ctype.h>

struct lexer_info g_lexer_info = {
    NULL,
    GENERAL,
    GENERAL,
    0,
    0,
    NULL,
    0
};

void skip_class(int (*classifier)(int c), const char *string, size_t *cursor)
{
    while (classifier(string[*cursor]))
        (*cursor)++;
}

static int look_ahead(const char *script, size_t size)
{
    if (g_lexer_info.exp_context == IN_SQUOTE)
    {
        return look_ahead_squote(size);
    }
    else
    {
        return look_ahead_keywords(script, size);
    }
}

static struct token_info lex_accumulator(struct token_info res, struct string *string)
{
    res.type = tokenify(string->data);
    if (res.type == T_WORD || g_lexer_info.last_context != GENERAL)
    {
        res = lex_command(res, string);
    }
    else
    {
        res = lex_keywords(res, string);
    }
    g_lexer_info.exp_context = GENERAL;
    g_lexer_info.last_context = GENERAL;
    return res;
}

/* MAIN LEXER */
struct token_info tokenify_next(const char *script, size_t size)
{
    struct token_info res = { 0, NULL };

    /* CHECK IF FINISHED TO READ THE SCRIPT */
    if (script[g_lexer_info.pos] == '\0' || g_lexer_info.pos >= size)
    {
        res.type = T_EOF;
        return res;
    }

    /* INFOS FOR LEXER */
    struct string *accumulator = string_create();

    /* LEXER */
    skip_class(isblank, script, &g_lexer_info.pos);
    do
    {
        if (!detect_context(script[g_lexer_info.pos]))
            accumulator = string_append(accumulator, script[g_lexer_info.pos]);
        g_lexer_info.pos++;

        if (detect_first_seperator(accumulator))
            break;
    } while (look_ahead(script, size));

    return lex_accumulator(res, accumulator);
}

struct token_info look_forward_token(int i)
{
    struct token_info EOF = { T_EOF, NULL };

    if (g_lexer_info.token_list == NULL || g_lexer_info.array_pos + i < 0 ||
        g_lexer_info.array_pos + i >= g_lexer_info.token_list->size)
    {
        return EOF;
    }

    return g_lexer_info.token_list->data[g_lexer_info.array_pos + i];
}

struct token_info get_next_token(void)
{
    struct token_info EOF = { T_EOF, NULL };

    if (g_lexer_info.token_list == NULL ||
        g_lexer_info.array_pos >= g_lexer_info.token_list->size)
    {
        return EOF;
    }

    return g_lexer_info.token_list->data[g_lexer_info.array_pos];
}

struct token_info pop_token(void)
{
    struct token_info EOF = { T_EOF, NULL };

    if (g_lexer_info.token_list == NULL ||
        g_lexer_info.array_pos >= g_lexer_info.token_list->size)
    {
        return EOF;
    }

    return g_lexer_info.token_list->data[g_lexer_info.array_pos++];
}
