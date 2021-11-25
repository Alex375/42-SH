#ifndef INC_42_SH_TOKEN_VEC_H
#define INC_42_SH_TOKEN_VEC_H

#include <stddef.h>

#include "tokens.h"

struct tkvec
{
    size_t capacity;
    size_t size;
    struct token_info *data;
};

struct tkvec *tkvec_create();

struct tkvec *tkvec_append(struct tkvec *string, struct token_info tk);

void tkvec_free(struct tkvec *string);

#endif // INC_42_SH_TOKEN_VEC_H
