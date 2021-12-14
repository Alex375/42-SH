#include "lexer.h"

struct token_info lex_for(struct token_info res, struct string *string)
{
    if (g_lexer_info.for_context == VAR_FOR)
    {
        res.type = T_VAR;
        res.command = string_get(string);
        g_lexer_info.for_context = IN_FOR;
    }
    else if (is_token_seperator(res.type))
    {
        g_lexer_info.for_context = GENERAL_FOR;
        g_lexer_info.word_context = GENERAL;
    }
    else if (res.type == T_IN && g_lexer_info.for_context == IN_FOR)
    {
        g_lexer_info.for_context = GENERAL_FOR;
        g_lexer_info.word_context = IN_COMMAND;
    }
    else if (res.type == T_DO)
    {
        g_lexer_info.for_context = GENERAL_FOR;
        g_lexer_info.word_context = IN_COMMAND;
    }
    else
    {
        res.type = T_WORD;
        res.command = string_get(string);
    }

    return res;
}
