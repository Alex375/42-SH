#pragma once

#include "execution.h"

/*!
 * @brief Replicate the echo function
 * @param args Array of string containing arguments of echo
 * @note  Implemented arguments are -n and -e
 */
int echo(char **args);
/*!
 * @brief Replicate the exit function
 * @param args Array of string containing arguments of exit
 */
int xexit(char **args);

/*!
 * @brief Replicate the cd function
 * @param args Array of string containing arguments of cd
 */
int cd(char **args);

/*!
 * @brief Execute the builtins at the given index (call get_builtins_index to
 * get index)
 * @param index
 * @param args
 * @return Return the exit code of the executed command
 */
int exec_builtin(int index, char **args);

/*!
 * @brief Return the exit
 * @param function function name to be executed
 * @return Return -1 if the command is not found in the builtins list else id
 * return the index of the given command in the builtins list
 */
int get_builins_index(char *function);
