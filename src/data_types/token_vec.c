#include "token_vec.h"
#include "xalloc.h"

struct tkvec *tkvec_create()
{
    struct tkvec *res = xcalloc(1, sizeof(struct tkvec));
    res->capacity = 16;
    res->data = xcalloc(res->capacity, sizeof(char));
    return res;
}

struct tkvec *tkvec_append(struct tkvec *tkvec, struct token_info tk)
{
    if (tkvec->size == tkvec->capacity)
    {
        tkvec->capacity = tkvec->capacity * 2;
        tkvec->data = xrealloc(tkvec->data, tkvec->capacity * sizeof(struct tkvec));
    }
    tkvec->data[tkvec->size] = tk;
    tkvec->size++;
    return tkvec;
}

void tkvec_free(struct tkvec *tkvec)
{
    xfree(tkvec->data);
    xfree(tkvec);
}
