#ifndef INC_42_SH_TYPE_H
#define INC_42_SH_TYPE_H

#if __APPLE__
#    include "xfnmatch.h"
#else
#    define _GNU_SOURCE
#    include <fnmatch.h>
#endif

#include <stddef.h>

#include "token_vec.h"
#include "tokens.h"
#include "xstring.h"

enum word_context
{
    GENERAL,
    IN_COMMAND
};

enum hard_expansion
{
    GENERAL_EXP_HARD,
    IN_SQUOTE_EXP,
    IN_ESCAPE_EXP
};

enum soft_expansion
{
    GENERAL_EXP_SOFT,
    IN_DQUOTE
};

enum for_context
{
    GENERAL_FOR,
    VAR_FOR,
    IN_FOR,
};

enum var_context
{
    GENERAL_VAR,
    IN_VAR,
    IN_VAR_NAME
};

enum redir_context
{
    GENERAL_REDIR,
    IN_REDIR
};

enum fun_context
{
    GENERAL_FUN,
    IN_FUN_NAME
};

/**
** @brief                   Global info for the lexer
** @param nb_token          The number of token in the token converter.
** @param nb_separator      The number of separator in the separator list.
** @param pos               Index pos in the script string.
** @param token_convertor   Array for converting string into token.
** @param separator         Array of useful seperator for the lexer.
*/
struct lexer_info
{
    struct tkvec *token_list;
    enum fun_context fun_context;
    enum redir_context redir_context;
    enum soft_expansion last_soft;
    enum soft_expansion soft_expansion;
    enum var_context var_context;
    enum for_context for_context;
    enum word_context word_context;
    enum hard_expansion exp_context;
    enum hard_expansion last_exp_context;
    size_t array_pos;
    size_t pos;
    char *script;
    size_t script_size;
};

struct words_converter
{
    size_t nb_token;
    size_t nb_separator;
    char *token_converter[30];
    char *separator[20];
};

/**
** @brief                   Global variable that store the infos about the
*lexer.
*/
extern struct lexer_info g_lexer_info;

/**
** @brief                   Check if a string is a separator.
*/
int separatorify(const char *token_str);

int is_token_seperator(enum token token);

enum token tokenify(const char *token_str);

int is_ionumber(struct token_info res, struct string *string);

int look_ahead_var(const char *script, size_t size);

int is_valid_var(const char *string);

struct token_info lex_varvalue(struct token_info res, struct string *string);

struct token_info lex_varname(struct token_info res, struct string *string);

struct token_info lex_var(struct token_info res, struct string *string);

struct token_info lex_ionumber(struct token_info res, struct string *string);

int check_special(struct string *accumulator, char next_char);

int look_ahead_keywords(const char *script, size_t size);

struct token_info lex_for(struct token_info res, struct string *string);

struct token_info lex_keywords(struct token_info res, struct string *string);

int is_command(struct token_info res);

struct token_info lex_command(struct token_info res, struct string *string);

int skip_character(char c);

struct token_info lex_fun(struct token_info res, struct string *string);

void context_update(struct token_info res);

int detect_context(char c);

int look_ahead_dquote(const char *script, size_t size, size_t acu_size);

int look_ahead_squote(size_t size);

void lexer_start(char *script, size_t size, long end);

void lexer_reset();

/**
** @brief                   Token-ify the next element of a script without
*popping.
** @param script            Script to be lex.
** @param size              Size in character of the script.
*/
struct token_info get_next_token(void);

/**
** @brief                   Token-ify and pop the next element of a script.
** @param script            Script to be lex.
** @param size              Size in character of the script.
*/
struct token_info pop_token(void);

struct token_info tokenify_next();

struct token_info look_forward_token(int i);

#endif // INC_42_SH_TYPE_H
