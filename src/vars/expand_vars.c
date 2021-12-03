#include "vars.h"

#include "xalloc.h"
#include "xparser.h"
#include "xstrdup.h"

char **expand_vars_vect(struct vars_vect *vars, struct tok_vect *tok_vect);
