#ifndef INC_42_SH_VECTOR_TOKENS_H
#define INC_42_SH_VECTOR_TOKENS_H

struct tok_vect
{
    int len;
    int cap;
    struct token_info *list;
};

struct tok_vect *init_tok_vect();

int add_word_vect(struct tok_vect *tok_vect);

#endif //INC_42_SH_VECTOR_TOKENS_H
