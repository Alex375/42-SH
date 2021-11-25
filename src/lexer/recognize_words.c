#include <string.h>

#include "lexer.h"

struct words_converter converter = {
    27 ,
    16,
    { "if", "then", "elif", "else", "fi", "while", "until", "for", "do", "done", "!", "||", "&&", "\n", ";", "{", "}",
      "(", ")", "|", ">", "<", ">&", "<&", ">>", "<>", ">|" },
    { "||", "&&", "\n", ";", "(", ")", "|", " ", "\0", ">", "<", ">&", "<&", ">>", "<>", ">|"  }
};

int separatorify(const char *token_str)
{
    if (token_str == NULL || token_str[0] == '\0')
        return -1;
    size_t i = 0;

    /* CHECK IF WE CAN CONVERT INTO A KNOWN SEPARATOR */
    while (i < converter.nb_separator
           && strcmp(token_str, converter.separator[i]) != 0)
        i++;

    /* UNKOWN SEPARATOR */
    if (i >= converter.nb_separator)
        return -1;

    /* KNOWN SEPARATOR */
    return i;
}

enum token tokenify(const char *token_str)
{
    if (token_str == NULL || token_str[0] == '\0')
        return -1;
    size_t i = 0;

    /* CHECK IF WE CAN CONVERT INTO A KNOWN TOKEN */
    while (i < converter.nb_token
           && strcmp(converter.token_converter[i], token_str) != 0)
        i++;

    /* UNKOWN TOKEN */
    if (i >= converter.nb_token)
        return T_WORD;

    /* KNOWN TOKEN */
    return i;
}

int look_ahead_token(struct string *accumulator, char next_char)
{
    if (g_lexer_info.exp_context == IN_SQUOTE)
    {
        return 0;
    }

    int token;

    if (g_lexer_info.pos + 1 < g_lexer_info.script_size)
    {
        accumulator = string_append(accumulator, next_char);
        token = separatorify(accumulator->data);
        if (token != -1)
        {
            g_lexer_info.pos++;
            return 1;
        }
        accumulator = string_pop(accumulator, NULL);
    }

    token = separatorify(accumulator->data);
    if (token == -1)
        return 0;

    return 1;
}
