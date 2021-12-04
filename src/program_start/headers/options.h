//
// Created by Alexandre Josien on 23/11/2021.
//

#ifndef INC_42_SH_OPTIONS_H
#define INC_42_SH_OPTIONS_H

#include <stddef.h>

/**
** @brief                   Struct representing the arguments of the program
** @param print             True if print is active
** @param help              True if help is active
** @param verbose           True if verbose is active
** @param script            Script to be executed (string NULL terminated)
** @param argc              Argument count of the script (including argv[0])
** @param argv              List of argument for the script (containing argv[0])
*/
struct options
{
    int print;
    int help;
    int verbose;
    char *script;
    int argc;
    char **argv;
};

/**
** @brief                   Print program usage
*/
void print_usage(void);
/**
** @brief                   Set up the options by reading argv with getopt_long
** @param options           Option struct to be setup
** @param argc              Program argc
** @param argv              Program argv
*/
void get_option(struct options *options, int argc, char **argv);

/*!
 * @brief Pre parsing for option
 * @param argc Argc of the program
 * @param argv Argv of the program
 * @return Return the index ending the option part
 */
int preparseopt(int argc, char **argv);

/*!
 * @brief Duplicate an array
 * @param argc argc
 * @param argv argv
 * @return Return the duplicated array (dynamically xallocated)
 */
char **dupplicate(int argc, char **argv);

#endif // INC_42_SH_OPTIONS_H
