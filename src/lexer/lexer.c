#include "lexer.h"

#include <ctype.h>

struct lexer_info g_lexer_info = { NULL,
                                   GENERAL_EVAL,
                                   GENERAL_CASE,
                                   GENERAL_FUN,
                                   GENERAL_REDIR,
                                   GENERAL_EXP_SOFT,
                                   GENERAL_EXP_SOFT,
                                   GENERAL_VAR,
                                   GENERAL_FOR,
                                   GENERAL,
                                   GENERAL_EXP_HARD,
                                   GENERAL_EXP_HARD,
                                   0,
                                   0,
                                   NULL,
                                   0 };

void skip_class(int (*classifier)(int c), const char *string, size_t *cursor)
{
    while (classifier(string[*cursor]))
        (*cursor)++;
}

static int look_ahead(const char *script, size_t size, struct string *acu)
{
    if (g_lexer_info.eval_context == IN_EVAL)
    {
        return look_eval(script, size);
    }
    else if (g_lexer_info.exp_context != GENERAL_EXP_HARD)
    {
        return look_ahead_squote(size);
    }
    else if (g_lexer_info.soft_expansion == IN_DQUOTE)
    {
        return look_ahead_dquote(script, size, acu->size);
    }
    else
    {
        return look_ahead_keywords(script, size);
    }
}

static struct token_info lex_accumulator(struct token_info res,
                                         struct string *string)
{
    if (res.type != T_ERROR && res.type != T_EVALEXPR)
        res.type = tokenify(string->data);
    context_update(res);

    if (res.type == T_EVALEXPR)
    {
        g_lexer_info.eval_context = GENERAL_EVAL;
        res.command = string_get(string);
    }
    else if (res.type == T_ERROR)
    {
        res.type = T_ERROR;
    }
    else if (g_lexer_info.for_context != GENERAL_FOR)
    {
        res = lex_for(res, string);
    }
    else if (res.type == T_D_PAREN || res.type == T_BACKQUOTE)
    {
        res = lex_sub(res);
    }
    else if (g_lexer_info.var_context == IN_VAR_NAME)
    {
        res = lex_varname(res, string);
    }
    else if (g_lexer_info.var_context != GENERAL_VAR
             && g_lexer_info.var_context != IN_VAR_INIT
             && g_lexer_info.last_exp_context != IN_SQUOTE_EXP)
    {
        res = lex_var(res, string);
    }
    else if (g_lexer_info.fun_context == IN_FUN_NAME)
    {
        res = lex_fun(res, string);
    }
    else if (is_ionumber(res, string))
    {
        res = lex_ionumber(res, string);
    }
    else if (is_command(res))
    {
        res = lex_command(res, string);
    }
    else
    {
        res = lex_keywords(res, string);
    }

    g_lexer_info.exp_context = GENERAL_EXP_HARD;
    g_lexer_info.last_exp_context = GENERAL_EXP_HARD;
    if (g_lexer_info.soft_expansion == GENERAL_EXP_SOFT)
        g_lexer_info.last_soft = GENERAL_EXP_SOFT;
    if (res.type == T_WORD)
        g_lexer_info.redir_context = GENERAL_REDIR;

    return res;
}

/* MAIN LEXER */
struct token_info tokenify_next(const char *script, size_t size)
{
    struct token_info res = { 0, NULL, 0 };

    if (g_lexer_info.soft_expansion != IN_DQUOTE
        && g_lexer_info.eval_context != IN_EVAL)
        skip_class(isblank, script, &g_lexer_info.pos);

    /* CHECK IF FINISHED TO READ THE SCRIPT */
    if (script[g_lexer_info.pos] == '\0' || g_lexer_info.pos >= size)
    {
        res.type = T_EOF;
        if (g_lexer_info.soft_expansion == IN_DQUOTE)
        {
            g_lexer_info.soft_expansion = GENERAL_EXP_SOFT;
            res.type = T_ERROR;
        }

        return res;
    }

    /* LEXER */
    int tmp;
    struct string *accumulator = string_create();
    do
    {
        if (script[g_lexer_info.pos] == '\0' || g_lexer_info.pos >= size)
        {
            res.type = T_EOF;
            if (g_lexer_info.soft_expansion == IN_DQUOTE)
            {
                g_lexer_info.soft_expansion = GENERAL_EXP_SOFT;
                res.type = T_ERROR;
            }
            break;
        }
        if (!skip_character(script[g_lexer_info.pos]))
            accumulator = string_append(accumulator, script[g_lexer_info.pos]);

        detect_context(script[g_lexer_info.pos]);
        g_lexer_info.pos++;

        tmp = check_special(accumulator, script[g_lexer_info.pos]);
        if (tmp != 0)
        {
            if (tmp == 2)
            {
                res.type = T_EVALEXPR;
                accumulator->size = 0;
            }
            else
                break;
        }

    } while (look_ahead(script, size, accumulator));

    if (g_lexer_info.soft_expansion == GENERAL_EXP_SOFT)
        res.is_space_after = isblank(script[g_lexer_info.pos])
            || script[g_lexer_info.pos] == '\n';

    if (g_lexer_info.soft_expansion != IN_DQUOTE
        && g_lexer_info.eval_context != IN_EVAL)
        skip_class(isblank, script, &g_lexer_info.pos);

    return lex_accumulator(res, accumulator);
}
