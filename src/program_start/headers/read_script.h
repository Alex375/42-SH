//
// Created by Alexandre Josien on 23/11/2021.
//

#ifndef INC_42_SH_READ_SCRIPT_H
#define INC_42_SH_READ_SCRIPT_H

#define BUFFER_SIZE 2048
#include <stddef.h>

/**
** @brief                   Read a script file
** @param filename          Filename of the script file to read
* the file content (automatically set to 0 at the beginning)
** @return                  Return the pointer to the string containing the
* file content dynamically allocated by the function (\0 is set at the end
* of buffer)
*/
char *read_script(char *filename);

#endif // INC_42_SH_READ_SCRIPT_H
