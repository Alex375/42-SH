#include <lexer.h>

int is_command(struct token_info res)
{
    if (g_lexer_info.word_context == IN_COMMAND
        && g_lexer_info.case_context == IN_CASE && res.type == T_ESAC
        && g_lexer_info.token_list->size > 0
        && g_lexer_info.token_list->data[g_lexer_info.token_list->size - 1].type
            != T_CASE)
    {
        return 0;
    }
    return res.type == T_WORD
        || g_lexer_info.last_exp_context != GENERAL_EXP_HARD
        || g_lexer_info.last_soft == IN_DQUOTE
        || (g_lexer_info.word_context == IN_COMMAND);
}

struct token_info lex_command(struct token_info res, struct string *string)
{
    if (g_lexer_info.exp_context == IN_SQUOTE_EXP)
    {
        res.type = T_ERROR;
        res.command = NULL;
        string_free(string);
    }
    else
    {
        if (g_lexer_info.redir_context == GENERAL_REDIR)
            g_lexer_info.word_context = IN_COMMAND;
        if (g_lexer_info.var_context == IN_VAR_INIT)
        {
            g_lexer_info.var_context = GENERAL_VAR;
            g_lexer_info.word_context = GENERAL;
        }
        if (g_lexer_info.case_context == IN_CASE)
        {
            g_lexer_info.word_context = GENERAL;
        }

        res.type = T_WORD;
        res.command = string_get(string);
    }
    return res;
}
