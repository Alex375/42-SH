#include "lexer.h"

#include <string.h>

#include "xalloc.h"

struct lexer_info g_lexer_info = { 15,
                                   3,
                                   0,
                                   { "if", "then", "else", "fi", "!", "||",
                                     "&&", "\n", ";", "'", "{", "}", "(", ")",
                                     "|" },
                                   { ';', '\n', '\0' } };

/**
** @brief                   Check if a char is a separator
** @param c                 Char to be compared.
*/
static int is_separator(char c)
{
    size_t i = 0;

    /* CHECK IF WE CAN CONVERT INTO A KNOWN SEPARATOR */
    while (i < g_lexer_info.nb_separator && c != g_lexer_info.separator[i])
        i++;

    /* UNKOWN SEPARATOR */
    if (i >= g_lexer_info.nb_separator)
        return 0;

    /* KOWN SEPARATOR */
    return 1;
}

/**
** @brief                   Token-ify a string. Return -1 if it cannot be token-ify
 *else it return the index in the token converter array
** @param token_str               String to be compared.
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

struct token_info get_next_token(const char *script, size_t size)
{
    struct token_info res = { 0, NULL };

    /* CHECK IF FINISHED TO READ THE SCRIPT */
    if (script[g_lexer_info.pos] == '\0' || g_lexer_info.pos >= size)
    {
        res.type = T_EOF;
        return res;
    }

    /* INFOS FOR LEXER */
    char *accumulator = xcalloc(size, sizeof(char));
    int token;
    size_t i = 0;

    /* LEXER */
    while ((token = tokenify(accumulator)) == -1)
    {
        /* COMMAND HANDLER */
        if ((is_separator(script[g_lexer_info.pos]) && token == -1 && i != 0)
            || g_lexer_info.pos >= size)
        {
            res.type = T_COMMAND;
            res.command = accumulator;
            return res;
        }
        /* ACCUMULATOR */
        if (i != 0 || script[g_lexer_info.pos] != ' ') /* Delete beginning spaces */
        {
            accumulator[i] = script[g_lexer_info.pos];
            i++;
        }
        g_lexer_info.pos++;
    }

    res.type = token;
    return res;
}
/*int main()
{
    char *test = "touch if";
    size_t size = 57;
    struct token_info token;
    while ((token = get_next_token(test, size)).type != T_EOF)
    {
        continue;
    }

    return 0;
}/*
