//
// Created by Alexandre Josien on 24/11/2021.
//

#ifndef INC_42_SH_EXECUTION_H
#define INC_42_SH_EXECUTION_H

#include "execution.h"
#include "parser.h"

struct pipeline
{
    int out;
    int fd[2];
};

int execute(char *cmd, struct pipeline *pipeline);
int exec_pipe(struct ast *left, struct ast *right, struct pipeline *pipeline);

#endif // INC_42_SH_EXECUTION_H
