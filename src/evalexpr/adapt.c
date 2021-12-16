#include "evalexpr.h"
#include "xalloc.h"
#include "string.h"
#include "vars.h"

char *adapt(char *line, int len)
{
    struct string *new_line = string_create();

    for (int i = 0; i < len; ++i)
    {
        if (isalpha(line[i]))
        {
            char *acc = xcalloc(1000, 1);
            int j = 0;
            while (isalpha(line[i]))
            {
                acc[j] = line[i];
                j++;
                i++;
            }

            char *val = get_var(acc, NULL);
            if (!*val)
            {
                val = xcalloc(2, 1);
                val[0] = '0';
            }


        }

    }
}
