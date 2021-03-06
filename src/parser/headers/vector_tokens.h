#ifndef INC_42_SH_VECTOR_TOKENS_H
#define INC_42_SH_VECTOR_TOKENS_H

#include "lexer.h"

struct tok_vect
{
    int len;
    int cap;
    struct token_info *list;
    struct ast **cmd_sub_list;
};

/**
** @brief               return 1 if the token type is possibly part of a word
*/
int is_part_word(enum token t);

/**
** @brief               alloc and inits the token vector
*/
struct tok_vect *init_tok_vect();

/**
** @brief               printing the ast obtain from a script
** @param tok_vect      token vector to append to
* @param quote_word    1 for redirs and assignation, transform the T_var
*                      in T_var_inquote to follow dash behavior
*/
int add_word_vect(struct tok_vect *tok_vect, int quote_word);

/**
** @brief              duplicate the vector structure
*/
struct tok_vect *dup_token_vect(struct tok_vect *vect);

/**
** @brief               free the vect structure
*/
void free_token_vect(struct tok_vect *vect);

#endif // INC_42_SH_VECTOR_TOKENS_H
