#include "lexer.h"

#include <ctype.h>


struct lexer_info g_lexer_info = {
    GENERAL,
    GENERAL_EXP,
    0,
};

void skip_class(int (*classifier)(int c), const char *string, size_t *cursor)
{
    while (classifier(string[*cursor]))
        (*cursor)++;
}

static int look_ahead(const char *script, size_t size)
{
    if (g_lexer_info.exp_context == IN_SQUOTE_EXP)
    {
        return look_ahead_squote(script, size);
    }
    else
    {
        return look_ahead_keywords(script, size);
    }
}

static struct token_info lex_accumulator(struct token_info res, struct string *string)
{
    res.type = tokenify(string->data);
    if (res.type == T_COMMAND)
    {
        res = lex_command(res, string);
    }
    else if (res.type == T_QUOTE)
    {
        res = lex_squote(res, string);
    }
    else
    {
        res = lex_keywords(res, string);
    }

    return res;
}

/* MAIN LEXER */
static struct token_info tokenify_next(const char *script, size_t size, int pop)
{
    struct token_info res = { 0, NULL };

    /* CHECK IF FINISHED TO READ THE SCRIPT */
    if (script[g_lexer_info.pos] == '\0' || g_lexer_info.pos >= size)
    {
        g_lexer_info.pos = 0;
        g_lexer_info.context = GENERAL;
        res.type = T_EOF;
        return res;
    }

    /* INFOS FOR LEXER */
    struct string *accumulator = string_create();
    size_t pos_backup = g_lexer_info.pos;

    /* LEXER */
    skip_class(isblank, script, &g_lexer_info.pos);
    do
    {
        accumulator = string_append(accumulator, script[g_lexer_info.pos]);
        g_lexer_info.pos++;

        if (detect_first_seperator(accumulator))
            break;
    } while (look_ahead(script, size));

    /* IF WE DO GET NEXT TOKEN WE DONT POP*/
    if (!pop)
    {
        g_lexer_info.pos = pos_backup;
    }

    return lex_accumulator(res, accumulator);
}

struct token_info get_next_token(const char *script, size_t size)
{
    return tokenify_next(script, size, 0);
}

struct token_info pop_token(const char *script, size_t size)
{
    return tokenify_next(script, size, 1);
}
