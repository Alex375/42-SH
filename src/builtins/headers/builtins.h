#pragma once

#include "execution.h"

/*!
 * @brief Replicate the echo function
 * @param args Array of string containing arguments of echo
 * @note  Implemented arguments are -n and -e
 */
int echo(char **args);
int exec_builtin(int index, char **args, struct pipeline *pipeline);
int is_builins(char *function);
