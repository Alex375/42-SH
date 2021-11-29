#ifndef INC_42_SH_PARSER_H
#define INC_42_SH_PARSER_H

#include "lexer.h"

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
    char **cmd_arg;
    struct list_redir *redirs;
};

/**
** @brief                   AST_IF ast member.
*/
struct n_if
{
    struct ast *condition;
    struct ast * true_c;
    struct ast * false_c;
};

/**
** @brief                   AST_CMD ast member.
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
** @brief                   AST_CMD ast member.
*/
struct n_command
{
    struct ast *ast;
    struct list_redir *redirs;
};

/**
** @brief                   AST_FOR ast member.
*/
struct n_for
{
    char *name;
    char **seq;
    struct ast *statement;
};

/**
** @brief               builds a n_binary node
** @param type          type to build
** @param left          left child ast
** @param right         right child ast
*/
struct ast *build_binary(enum AST_TYPE type, struct ast *left,
                         struct ast *right);

/**
** @brief               builds a n_if node
** @param condition     condition child ast
** @param left          left child ast
** @param right         right child ast
*/
struct ast *build_if(struct ast *condition, struct ast * true_c,
                     struct ast * false_c);

/**
** @brief               builds a n_s_cmd node
** @param cmd           name of the command
** @param cmd_arg       string list containing all the command arguments
 *                      separated by spaces.
** @param redirs        list of redirection in the command
*/
struct ast *build_s_cmd(char *cmd, char **cmd_arg, struct list_redir *redirs);

/**
** @brief               builds a n_command node
** @param ast           child ast
** @param redirs        list of redirections
*/
struct ast *build_cmd(struct ast *ast, struct list_redir *redirs);

/*!
** @brief               builds a n_for node
** @param name          name of the variable of for
** @param seq           list on what the name variable is going to
 *                      take it's values
** @param statement     ast inside the for
 */
struct ast *build_for(char *name, char **seq, struct ast *statement);

#include <stddef.h>

#define ERROR_PARSING 127
#define ERROR_EMPTY_EOF 69

/**
** @brief               executes all the lines of a script
*                      (crash if a parsing error is found)
** @param script        string containing block
** @param size          len of script parameter
** @return              Return le exit code of the last executed command
*/
int exec_script(char *script, size_t size);

/**
** @brief               printing the ast obtain from a script
** @param ast           Ast to be printed
*/
void ast_pretty_print(struct ast *ast);

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
 * @param redirs        list of redir to append to
*/
void *parse_redirs(struct list_redir **redirs);

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
** @brief               Parsing a while rule and an until rule
**                      (cf sh_grammar.txt)
 * @param tokT          either WHILE or UNTIL
*/
struct ast *parse_while_until_rule(enum token tokT);

/**
** @brief               Parsing a for rule
**                      (cf sh_grammar.txt)
*/
struct ast *parse_for_rule();

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
** @param list_type     0 if in a list and 1 if in compound
*/
int check_ender_token(int in_compound);

/**
** @brief               return 1 if there is a redir token not following
 *                      the grammar (sets errno)
**                      (cf sh_grammar.txt)
*/
int err_redir();

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
