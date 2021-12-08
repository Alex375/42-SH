#ifndef INC_42_SH_VARS_H
#define INC_42_SH_VARS_H

#include "vector_tokens.h"
#include "func.h"

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
    char **at;
    int argc;
};

/**
** @brief               the global variable
*/
struct context
{
    struct vars_vect *vars;
    struct fc_vect *fcs;
};

/**
** @brief               inits context global var
*/
struct context *init_context();

/**
** @brief               inits vars global var
*/
struct vars_vect *init_vars_vect();

/**
** @brief               add a variable to the global variable vars
*/
void add_var(char *name, char *value);

/**
** @brief               add a variable to variable vars givven as arg
*/
void add_var_var(struct vars_vect *vars, char *name, char *value);


void push_pop_arg(struct vars_vect *var, int pop);

/**
** @brief               add a part of @ variable to the global variable vars
*/
void set_var_at(char *value, int i);

/**
** @brief               add a variable to the global variable vars casting the
*                       int to a char
*/
void set_var_int(char *name, long value);

/**
** @brief               setter for argc
*/
void set_vars_argc(int argc);

/**
** @brief               getter for argc
*/
int get_vars_argc();

/**
** @brief               gets value of a variable in the global variable vars
*                      if name does not exist, it returns "";
*                      i for accesing @ var
*/
char *get_var(char *name, int *i_at);

/**
** @brief               saves the arguments related vars in a vect
*/
void save_arg_var();

/**
** @brief               free vars variable
*/
void free_vars(struct vars_vect *vars);

/**
** @brief               free context variable
*/
void free_context();

//////////////////

/**
** @brief               tansforms a list of tokens into a char **
*/
char **expand_vars_vect(struct tok_vect *tok_vect);

/*!
 * @brief Compare an int and en variable
 * @param var name of the variable
 * @param n int to compare
 * @return Return the comparison result between var and n (return false if var
 * is not found or conversion failed)
 */
int int_eq_var(char *var, int n);

#endif // INC_42_SH_VARS_H
