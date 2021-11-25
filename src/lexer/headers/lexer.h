#ifndef INC_42_SH_TYPE_H
#define INC_42_SH_TYPE_H

#include <stddef.h>

#include "token_vec.h"
#include "tokens.h"
#include "xstring.h"

enum expansion_context
{
    GENERAL,
    IN_SQUOTE
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
    enum expansion_context exp_context;
    enum expansion_context last_context;
    size_t array_pos;
    size_t pos;
    char *script;
    size_t script_size;
};

struct words_converter
{
    size_t nb_token;
    size_t nb_separator;
    char *token_converter[17];
    char *separator[11];
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

enum token tokenify(const char *token_str);

int detect_first_seperator(struct string *accumulator);

int look_ahead_keywords(const char *script, size_t size);

struct token_info lex_keywords(struct token_info res, struct string *string);

struct token_info lex_command(struct token_info res, struct string *string);

int detect_context(char c);

int look_ahead_squote(size_t size);

void lexer_start(char *script, size_t size);

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
