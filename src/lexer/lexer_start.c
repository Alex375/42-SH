#include "lexer.h"

void lexer_start(char* script, size_t size)
{
    if (g_lexer_info.token_list != NULL)
        return;

    g_lexer_info.script = script;
    g_lexer_info.script_size = size;
    g_lexer_info.token_list = tkvec_create();

    struct token_info token;
    while ((token = tokenify_next(script, size)).type != T_EOF)
    {
        g_lexer_info.token_list = tkvec_append(g_lexer_info.token_list, token);
    }
}

void lexer_reset()
{
    g_lexer_info.array_pos = 0;
    g_lexer_info.pos = 0;
    g_lexer_info.exp_context = GENERAL;
    g_lexer_info.last_context = GENERAL;
    g_lexer_info.script = NULL;
    g_lexer_info.script_size = 0;
    tkvec_free(g_lexer_info.token_list);
    g_lexer_info.token_list = NULL;
}
