#include "lexer.h"

struct token_info look_forward_token(int i)
{
    struct token_info EOF = { T_EOF, NULL };
    int new_pos = g_lexer_info.array_pos + i;
    if (g_lexer_info.token_list == NULL || new_pos < 0
        || g_lexer_info.array_pos + i >= g_lexer_info.token_list->size)
    {
        return EOF;
    }

    return g_lexer_info.token_list->data[g_lexer_info.array_pos + i];
}

struct token_info get_next_token(void)
{
    struct token_info EOF = { T_EOF, NULL };

    if (g_lexer_info.token_list == NULL
        || g_lexer_info.array_pos >= g_lexer_info.token_list->size)
    {
        return EOF;
    }

    return g_lexer_info.token_list->data[g_lexer_info.array_pos];
}

struct token_info pop_token(void)
{
    struct token_info EOF = { T_EOF, NULL };

    if (g_lexer_info.token_list == NULL
        || g_lexer_info.array_pos >= g_lexer_info.token_list->size)
    {
        return EOF;
    }

    return g_lexer_info.token_list->data[g_lexer_info.array_pos++];
}
