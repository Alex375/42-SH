//
// Created by Alexandre Josien on 24/11/2021.
//

#ifndef INC_42_SH_EXECUTION_H
#define INC_42_SH_EXECUTION_H

#include "execution.h"
#include "parser.h"

/*!
 * @param out State param of the pipeline
 * @param fd  File descriptors arrays for (piped if currently use)
 * @note `out` states :
 * @note - -1 : no pipeline in use no redirection needed
 * @note - 0  : pipeline in use for exit stdin shall be redir in pipe exit
 * @note - 1  : pipeline in use for entry stoud shall be redir in pipe entry
 */
struct pipeline
{
    int out;
    int fd[2];
};

/*!
 *
 * @param cmd       Command string to be executed
 * @param args      Arguments string associated with the command
 * @param pipeline  Pipeline argument (give information about redir and
 * pipeline)
 * @return          Return the exit command of the executed command
 */
int execute(char *cmd, char *args, struct pipeline *pipeline);

/*!
 *
 * @param left     Left ast node of the pipe node to be executed
 * @param right    Right ast node of the pipe node to be executed
 * @param pipeline Pipeline argument (give information about redir and
 * pipeline) (value will be used in the function and reset as the function exit)
 * @return         Return the exit code of the left node execution
 */
int exec_pipe(struct ast *left, struct ast *right, struct pipeline *pipeline);

#endif // INC_42_SH_EXECUTION_H
