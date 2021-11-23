#include "lexer.h"

#include <string.h>

#include "xstring.h"

struct lexer_info g_lexer_info = {
    15,
    10,
    0,
    { "if", "then", "else", "fi", "!", "||", "&&", "\n", ";", "'", "{", "}",
      "(", ")", "|" },
    { "||", "&&", "\n", ";", "'", "{", "}", "(", ")", "|" }
};

/**
** @brief                   Check if a char is a separator
** @param c                 Char to be compared.
*/
static int separatorify(char *token_str)
{
    if (token_str == NULL || token_str[0] == '\0')
        return -1;
    size_t i = 0;

    /* CHECK IF WE CAN CONVERT INTO A KNOWN SEPARATOR */
    while (i < g_lexer_info.nb_separator
           && strcmp(token_str, g_lexer_info.separator[i]) != 0)
        i++;

    /* UNKOWN SEPARATOR */
    if (i >= g_lexer_info.nb_separator)
        return -1;

    /* KOWN SEPARATOR */
    return i;
}

/**
** @brief                   Token-ify a string. Return -1 if it cannot be
                            *token-ify else it return the index in the token converter array
** @param token_str         String to be compared.
*/
static int tokenify(char *token_str)
{
    if (token_str == NULL || token_str[0] == '\0')
        return -1;
    size_t i = 0;

    /* CHECK IF WE CAN CONVERT INTO A KNOWN TOKEN */
    while (i < g_lexer_info.nb_token
           && strcmp(g_lexer_info.token_converter[i], token_str) != 0)
        i++;

    /* UNKOWN TOKEN */
    if (i >= g_lexer_info.nb_token)
        return -1;

    /* KNOWN TOKEN */
    return i;
}

/**
** @brief                   Look-ahead for a separator
*/
static enum token look_ahead(const char *script, size_t size)
{
    size_t i = g_lexer_info.pos;

    if (i >= size)
        return T_COMMAND;

    struct string *accumulator = string_create();
    int token;

    accumulator = string_append(accumulator, script[i]);
    if ((token = separatorify(accumulator->data)) != -1)
    {
        string_free(accumulator);
        return token;
    }

    i++;
    accumulator = string_append(accumulator, script[i]);
    if ((token = separatorify(accumulator->data)) != -1)
    {
        string_free(accumulator);
        return token;
    }

    string_free(accumulator);
    return T_COMMAND;
}

/* MAIN LEXER */
static struct token_info tokenify_next(const char *script, size_t size, int pop)
{
    struct token_info res = { 0, NULL };

    /* CHECK IF FINISHED TO READ THE SCRIPT */
    if (script[g_lexer_info.pos] == '\0' || g_lexer_info.pos >= size)
    {
        g_lexer_info.pos = 0;
        res.type = T_EOF;
        return res;
    }

    /* INFOS FOR LEXER */
    struct string *accumulator = string_create();
    size_t pos_backup = g_lexer_info.pos;
    int token;

    /* LEXER */
    while ((token = tokenify(accumulator->data)) == -1)
    {
        /* COMMAND HANDLER AND LOOK AHEAD FOR SEPARATOR*/
        if ((token == -1 && look_ahead(script, size) != T_COMMAND && accumulator->size != 0)
            || g_lexer_info.pos >= size)
        {
            res.type = T_COMMAND;
            res.command = accumulator->data;
            return res;
        }

        /* ACCUMULATOR */
        if (accumulator->size != 0
            || script[g_lexer_info.pos] != ' ') /* Delete beginning spaces */
        {
            accumulator = string_append(accumulator, script[g_lexer_info.pos]);
        }
        g_lexer_info.pos++;
    }

    if (!pop)
        g_lexer_info.pos = pos_backup;

    string_free(accumulator);
    res.type = token;
    return res;
}

struct token_info get_next_token(const char *script, size_t size)
{
    return tokenify_next(script, size, 0);
}

struct token_info pop_token(const char *script, size_t size)
{
    return tokenify_next(script, size, 1);
}
