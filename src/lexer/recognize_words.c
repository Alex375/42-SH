#include <string.h>

#include "lexer.h"

struct words_converter converter = {
    17,
    11,
    { "if", "then", "elif", "else", "fi", "!", "||", "&&", "\n", ";",  "{",
      "}", "(", ")", "|", ">", "<" },
    { "||", "&&", "\n", ";", "(", ")", "|", " ", "\0", ">", "<" }
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

int detect_first_seperator(struct string *accumulator)
{
    if (g_lexer_info.exp_context == IN_SQUOTE)
    {
        return 0;
    }

    int token = separatorify(accumulator->data);
    if (token == -1)
        return 0;

    return 1;
}
