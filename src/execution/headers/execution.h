//
// Created by Alexandre Josien on 24/11/2021.
//

#ifndef INC_42_SH_EXECUTION_H
#define INC_42_SH_EXECUTION_H

#include "execution.h"
#include "xparser.h"
#include "vars.h"

/*!
 * @brief Execute a command including builtins
 * @param cmd       Command string to be executed
 * @param args      Arguments string associated with the command
 * @return          Return the exit command of the executed command
 */
int execute(char **args);

/*!
 * @brief Creating a pipeline between to ast node and execute both nodes
 * @param left     Left ast node of the pipe node to be executed
 * @param right    Right ast node of the pipe node to be executed
 * @return         Return the exit code of the left node execution
 */
int exec_pipe(struct ast *left, struct ast *right, struct vars_vect *vars);

/*!
 * @brief           Apply redirections, execute ast under redirections and
 * unapply redirections.
 * @param ast       Ast to be executed under redirections
 * @param listRedir List of redirections to be apply
 * @return          Return the exit code of the executed ast
 */
int exec_redirs(struct ast *ast, struct list_redir *listRedir, struct vars_vect *vars);

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

#endif // INC_42_SH_EXECUTION_H
