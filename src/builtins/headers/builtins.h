#pragma once

/*!
 * @brief Replicate the echo function
 * @param args String containing arguments of echo
 * @note  Implemented arguments are -n and -e
 */
void echo(char *args);
int exec_builtin(int index, char *args);
int is_builins(char *function);
