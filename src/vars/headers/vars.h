#ifndef INC_42_SH_VARS_H
#define INC_42_SH_VARS_H

#include "vector_tokens.h"

struct var
{
    char *name;
    char *value;
};

struct vars_vect
{
    int len;
    int cap;
    struct var *vars;
};

/**
** @brief               alloc and inits the token vector
*/
struct vars_vect *init_vars_vect();

/**
** @brief               add a variable to the global variable vars
*/
void add_var(char *name, char *value);

/**
** @brief               gets value of a variable in the global variable vars
 *                      if name does not exist, it returns "";
*/
char *get_var(char *name);

//////////////////

/**
** @brief               tansforms a list of tokens into a char **
*/
char **expand_vars_vect(struct tok_vect *tok_vect);

#endif // INC_42_SH_VARS_H
