#ifndef INC_42_SH_PARSER_H
#define INC_42_SH_PARSER_H

/**
** @brief                   Describe the node type in ast.
*/
typedef enum AST_TYPE
{
    AST_CMD,
    AST_IF,
    AST_WHILE,
    AST_FOR,
    AST_CASE,
    AST_UNTIL,
    AST_PIPE,
    AST_REDIR,
    AST_FUNC,
    AST_LIST,
    AST_NOT,
    AST_AND,
    AST_OR,
    AST_BRACKET,
    AST_PARENTH
} AST_TYPE;

/**
** @brief                   A flexible ast member.
*/
typedef struct ast
{
    AST_TYPE type;
    void *t_ast;
} ast;

/**
** @brief                   <<AST_CMD>> ast member.
*/
typedef struct n_cmd
{
    char *cmd_line;
} n_cmd;


/**
** @brief                   <<AST_IF>> ast member.
*/
typedef struct n_if
{
    ast *condition;
    ast *true;
    ast *false;
} n_if;

/**
** @brief                   <<AST_AND, AST_OR>> ast member.
*/
typedef struct n_binary
{
    ast *left;
    ast *right;
} n_binary;

#include "lexer.h"

#include <stddef.h>

#define ERROR_PARSING 69
#define ERROR_EMPTY_EOF 420

/**
** @brief               printing the ast obtain from a script
** @param script        string containing block
** @param size          len of script parameter
*/
void ast_pretty_print(const char *script, size_t size);

/**
** @brief               Parsing an input (cf sh_grammar.txt)
** @param script        string containing block
** @param size          len of script parameter
*/
struct ast *parse_input(const char *script, size_t size);

/**
** @brief               Parsing a list (cf sh_grammar.txt)
** @param script        string containing block
** @param size          len of script parameter
*/
struct ast *parse_list(const char *script, size_t size);

/**
** @brief               Parsing an and / or (cf sh_grammar.txt)
** @param script        string containing block
** @param size          len of script parameter
*/
struct ast *parse_and_or(const char *script, size_t size);

/**
** @brief               Parsing an pipeline (cf sh_grammar.txt)
** @param script        string containing block
** @param size          len of script parameter
*/
struct ast *parse_pipeline(const char *script, size_t size);

/**
** @brief               Parsing a command (cf sh_grammar.txt)
** @param script        string containing block
** @param size          len of script parameter
*/
struct ast *parse_command(const char *script, size_t size);

/**
** @brief               Parsing a simple command (cf sh_grammar.txt)
** @param script        string containing block
** @param size          len of script parameter
*/
struct ast *parse_simple_command(const char *script, size_t size);

/**
** @brief               Parsing a shell command (cf sh_grammar.txt)
** @param script        string containing block
** @param size          len of script parameter
*/
struct ast *parse_shell_command(const char *script, size_t size);

/**
** @brief               Parsing an if rule (cf sh_grammar.txt)
** @param script        string containing block
** @param size          len of script parameter
*/
struct ast *parse_if_rule(const char *script, size_t size, int inElif);

/**
** @brief               Parsing a compound list (cf sh_grammar.txt)
** @param script        string containing block
** @param size          len of script parameter
*/
struct ast *parse_compound(const char *script, size_t size);

/**
** @brief               calling lexer while newline (cf sh_grammar.txt)
** @param script        string containing block
** @param size          len of script parameter
*/
void skip_newlines(const char *script, size_t size);

#endif // INC_42_SH_PARSER_H
