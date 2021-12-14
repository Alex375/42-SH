//
// Created by alexandre.josien on 11/26/21.
//

#ifndef INC_42_SH_LAUNCH_PROGRAMM_H
#define INC_42_SH_LAUNCH_PROGRAMM_H

/*!
 * @brief  Read stdin
 * @return Return the stdin content
 */
char *read_stdin(void);
/*!
 * @brief Launch the program
 * @param argc
 * @param argv
 * @return Return exit code of the program
 */
int launch_program(int argc, char **argv);

#endif // INC_42_SH_LAUNCH_PROGRAMM_H
