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
** @brief                   <<AST_NOT>> ast member.
*/
typedef struct n_not
{
    ast *ast;
} n_not;

/**
** @brief                   <<AST_AND, AST_OR>> ast member.
*/
typedef struct n_binary
{
    ast *left;
    ast *right;
} n_binary;

#include "../lexer/headers/lexer.h"

#define ERROR_PARSING 69

/**
** @brief               Parsing an input (cf sh_grammar.txt)
** @param lex           The lexer output.
*/
struct ast *parse_input(struct lexer_list **lex);

/**
** @brief               Parsing a list (cf sh_grammar.txt)
** @param lex           The lexer output.
*/
struct ast *parse_list(struct lexer_list **lex);

/**
** @brief               Parsing an and / or (cf sh_grammar.txt)
** @param lex           The lexer output.
*/
struct ast *parse_and_or(struct lexer_list **lex);

/**
** @brief               Parsing an pipeline (cf sh_grammar.txt)
** @param lex           The lexer output.
*/
struct ast *parse_pipeline(struct lexer_list **lex);

/**
** @brief               Parsing a command (cf sh_grammar.txt)
** @param lex           The lexer output.
*/
struct ast *parse_command(struct lexer_list **lex);

/**
** @brief               Parsing a simple command (cf sh_grammar.txt)
** @param lex           The lexer output.
*/
struct ast *parse_simple_command(struct lexer_list **lex);

/**
** @brief               Parsing a shell command (cf sh_grammar.txt)
** @param lex           The lexer output.
*/
struct ast *parse_shell_command(struct lexer_list **lex);

/**
** @brief               Parsing an if rule (cf sh_grammar.txt)
** @param lex           The lexer output.
*/
struct ast *parse_if_rule(struct lexer_list **lex);

/**
** @brief               Parsing a compound list (cf sh_grammar.txt)
** @param lex           The lexer output.
*/
struct ast *parse_compound(struct lexer_list **lex);



#endif // INC_42_SH_PARSER_H
