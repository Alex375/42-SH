#ifndef INC_42_SH_FUNC_H
#define INC_42_SH_FUNC_H

struct fc
{
    char *name;
    struct ast *ast;
};

struct fc_vect
{
    int len;
    int cap;
    struct fc *fcs;
};

/**
** @brief               alloc and inits the token vector
*/
struct fc_vect *init_func_vect();

/**
** @brief               add a variable to the global variable fcs
*/
void add_fc(char *name, struct ast *ast);

/**
** @brief               gets the ast of func named name
*/
int get_fc(char *name, struct ast **ast);

/**
** @brief               free the fcs struct
*/
void free_fcs();


#endif // INC_42_SH_FUNC_H
