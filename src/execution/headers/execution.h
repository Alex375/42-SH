//
// Created by Alexandre Josien on 24/11/2021.
//

#ifndef INC_42_SH_EXECUTION_H
#define INC_42_SH_EXECUTION_H

#include "execution.h"
#include "vars.h"
#include "xparser.h"

/*!
 * @brief Execute a command including builtins
 * @param args      Arguments string associated with the command
 * @return          Return the exit code of the executed command
 */
int execute(char **args);

/*!
 * @brief Execute a command
 * @param cmd       the ast node to execute
 * @return          Return the exit code of the executed command
 */
int exec_s_cmd(struct n_s_cmd *cmd);

/*!
 * @brief Creating a pipeline between to ast node and execute both nodes
 * @param left     Left ast node of the pipe node to be executed
 * @param right    Right ast node of the pipe node to be executed
 * @return         Return the exit code of the left node execution
 */
int exec_pipe(struct ast *left, struct ast *right);

/*!
 * @brief           Apply redirections, execute ast under redirections and
 * unapply redirections.
 * @param ast       Ast to be executed under redirections
 * @param listRedir List of redirections to be apply
 * @return          Return the exit code of the executed ast
 */
int exec_redirs(struct ast *ast, struct list_redir *listRedir);

/*!
 * @brief           Chained list containing infos about the current redirections
 * @param temp_fd   Temporary fd saving the io_number
 * @param file_fd   FD of the opened file for the redirection
 * @param io_number Int formatted io_number of the redirection
 * @param next      Next redirections
 */
struct redir_info
{
    int temp_fd;
    int file_fd;
    int io_number;
    struct redir_info *next;
};

/*!
 * @brief Execut a subshell command (fork)
 * @param ast Ast to be executed in the subshell
 * @return Return the exitcode of the subshell command.
 */
int subhsell(struct ast *ast);

/*!
 * @brief           Execute a command including a previously define function
 * @param args      Arguments string associated with the command
 * @param fc_ast    ast to execute;
 * @return          Return the exit code of the executed command
 */
int exec_func(char **args, struct ast *fc_ast);

/*!
 * @brief           Execute a case rule
 * @param n_case    the node to treat
 * @return          Return the exit code of the executed command
 */
int treat_case(struct n_case *case_ast);

/*!
 * @brief           Execute commands and gets the stdout
 * @param ast       ast to execute
 * @param stdout_r  pointer to the string of stdout content
 * @return          Return the exit code of the executed command
 */
int get_stdout(struct ast *ast, char **stdout_r);

#endif // INC_42_SH_EXECUTION_H
