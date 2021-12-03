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

struct vars_vect *init_vars_vect();

void add_var(struct vars_vect *vars, char *name, char *value);

//////////////////

char **expand_vars_vect(struct vars_vect *vars, struct tok_vect *tok_vect);

#endif // INC_42_SH_VARS_H
