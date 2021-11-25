//
// Created by Alexandre Josien on 23/11/2021.
//

#ifndef INC_42_SH_OPTIONS_H
#define INC_42_SH_OPTIONS_H

#include <stddef.h>

/**
** @brief                   Struct representing the arguments of the program
** @param nb_command        Number of commands if commands option is active (0
* otherwise)
** @param command           List of commands if option is active (NULL
* otherwise)
** @param print             True if print is active
** @param help              True if help is active
** @param verbose           True if verbose is active
** @param nb_script         Number of scripts files passed as arguments
** @param scripts           List of script file
*/
struct options
{
    size_t nb_command;
    char **commands;
    int print;
    int help;
    int verbose;
    size_t nb_script;
    char **scripts;
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
int get_option(struct options *options, int argc, char **argv);

#endif // INC_42_SH_OPTIONS_H
