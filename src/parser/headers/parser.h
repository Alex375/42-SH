#ifndef INC_42_SH_PARSER_H
#define INC_42_SH_PARSER_H

#include "lexer.h"
#include "xstring.h"

/**
** @brief                   Describe the node type in ast.
*/
enum AST_TYPE
{
    AST_S_CMD,
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
};

/**
** @brief                   A flexible ast member.
*/
struct ast
{
    enum AST_TYPE type;
    void *t_ast;
};

/**
** @brief                   <<AST_S_CMD>> ast member.
*/
struct n_s_cmd
{
    char *cmd;
    struct string *cmd_arg;
    struct list_redir *redirs;
};

/**
** @brief                   <<AST_IF>> ast member.
*/
struct n_if
{
    struct ast *condition;
    struct ast * true;
    struct ast * false;
};

/**
** @brief                   <<AST_CMD>> ast member.
*/
struct n_binary
{
    struct ast *left;
    struct ast *right;
};

/**
** @brief                   chained list of redirections.
*/
struct list_redir
{
    char *ionumber;
    enum token redir_type;
    char *word;
    struct list_redir *next;
};

/**
** @brief                   <<AST_CMD>> ast member.
*/
struct n_command
{
    struct ast *ast;
    struct list_redir *redirs;
};

#include <stddef.h>

#define ERROR_PARSING 69
#define ERROR_EMPTY_EOF 420

/**
** @brief               printing the ast obtain from a script
** @param script        string containing block
** @param size          len of script parameter
*/
void ast_pretty_print(char *script, size_t size);

/**
** @brief               start the parsing of a script
** @param script        string containing block
** @param size          len of script parameter
*/
struct ast *start_parse(char *script, size_t size);

/**
** @brief               Parsing an input (cf sh_grammar.txt)
*/
struct ast *parse_input();

/**
** @brief               Parsing a list (cf sh_grammar.txt)
*/
struct ast *parse_list();

/**
** @brief               Parsing an and / or (cf sh_grammar.txt)
*/
struct ast *parse_and_or();

/**
** @brief               Parsing an pipeline (cf sh_grammar.txt)
*/
struct ast *parse_pipeline();

/**
** @brief               Parsing a command (cf sh_grammar.txt)
*/
struct ast *parse_command();

/**
** @brief               Parsing a list of redirections (cf sh_grammar.txt)
*/
struct list_redir *parse_redirs();

/**
** @brief               Parsing a simple command (cf sh_grammar.txt)
*/
struct ast *parse_simple_command();

/**
** @brief               Parsing a shell command (cf sh_grammar.txt)
*/
struct ast *parse_shell_command();

/**
** @brief               Parsing an if rule (cf sh_grammar.txt)
** @param inElif        1 if you encountered an elif to pervent
**                      eating the fi token
*/
struct ast *parse_if_rule(int inElif);

/**
** @brief               Parsing a compound list (cf sh_grammar.txt)
*/
struct ast *parse_compound();

/**
** @brief               calling lexer while newline (cf sh_grammar.txt)
*/
void skip_newlines();

/**
** @brief               return 1 if the recursive grammar element should
**                      stop because of an unexpected token (cf sh_grammar.txt)
*/
int check_ender_token();

/**
** @brief               return 1 if the next tokens are a redirection
**                      (cf sh_grammar.txt)
*/
int is_redir();

/**
** @brief               adds @a new_redir to the redirection list @a redirs
**                      (cf sh_grammar.txt)
*/
void add_to_redir_list(struct list_redir **redirs,
                       struct list_redir *new_redir);

#endif // INC_42_SH_PARSER_H
