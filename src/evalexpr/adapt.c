#include "evalexpr.h"
#include "string.h"
#include "vars.h"
#include "xalloc.h"

char *adapt(char *line, int len)
{
    struct string *new_line = string_create();

    for (int i = 0; i < len; ++i)
    {
        if (isalpha(line[i]))
        {
            char *acc = xcalloc(1000, 1);
            int j = 0;
            while (i < len && isalpha(line[i]))
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

            xfree(acc);

            string_concat(new_line, val);

            i--;
        }
        else
        {
            char *acc = xcalloc(10000, 1);
            int j = 0;
            while (i < len && !isalpha(line[i]))
            {
                acc[j] = line[i];
                j++;
                i++;
            }

            xfree(acc);

            string_concat(new_line, acc);

            i--;
        }
    }

    return string_get(new_line);
}
