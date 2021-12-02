//
// Created by alexandre.josien on 11/26/21.
//

#ifndef INC_42_SH_LAUNCH_PROGRAMM_H
#define INC_42_SH_LAUNCH_PROGRAMM_H

/*!
 * @brief Execute first script of option
 * @return Return -1 if no script is parsed in option else return the exit code
 * of the execution
 */
int launch_script(void);
/*!
 * @brief Execute first command of option
 * @return Return -1 if no command is parsed in option else return the exit code
 * of the execution
 */
int launch_command(void);

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
