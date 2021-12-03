#ifndef INC_42_SH_VECTOR_TOKENS_H
#define INC_42_SH_VECTOR_TOKENS_H

struct tok_vect
{
    int len;
    int cap;
    struct token_info *list;
};

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

#endif //INC_42_SH_VECTOR_TOKENS_H
