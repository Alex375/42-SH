//
// Created by Alexandre Josien on 24/11/2021.
//

#ifndef INC_42_SH_EXECUTION_H
#define INC_42_SH_EXECUTION_H

#include "execution.h"
#include "parser.h"

/*!
 * @brief Pipelin struct containing infos on pipline and redir
 * @param out State param of the pipeline
 * @param fd  File descriptors arrays for (piped if currently use)
 */
struct pipeline
{
    int fd[2];
};

/*!
 * @brief Execute a command including builtins
 * @param cmd       Command string to be executed
 * @param args      Arguments string associated with the command
 * @return          Return the exit command of the executed command
 */
int execute(char *cmd, char **args);

/*!
 * @brief Creating a pipeline between to ast node and execute both nodes
 * @param left     Left ast node of the pipe node to be executed
 * @param right    Right ast node of the pipe node to be executed
 * @return         Return the exit code of the left node execution
 */
int exec_pipe(struct ast *left, struct ast *right);

#endif // INC_42_SH_EXECUTION_H
