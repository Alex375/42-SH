#include "xalloc.h"

#include <err.h>
#include <string.h>

struct xmalloc_t *head = NULL;

static struct xmalloc_t *get_header(void *data)
{
    struct xmalloc_t *header = data;
    return header - 1;
}
static void check_xmalloc(struct xmalloc_t *res)
{
    if (res == NULL)
    {
        xfree_all();
        errx(1, "xalloc : memory exhausted");
    }
}

static struct xmalloc_t *init_xmalloc(struct xmalloc_t *res, size_t size)
{
    res->previous = NULL;
    res->next = head;
    res->allocation_size = size;
    if (head != NULL)
        head->previous = res;

    head = res;

    res++;

    return res;
}

void *xmalloc(size_t size)
{
    struct xmalloc_t *res = malloc(sizeof(struct xmalloc_t) + size);
    check_xmalloc(res);

    res = init_xmalloc(res, size);

    void *data = res;
    return data;
}

void *xcalloc(size_t n, size_t size)
{
    size_t total_n = n + sizeof(struct xmalloc_t) / size + 1;
    struct xmalloc_t *res = calloc(total_n, sizeof(struct xmalloc_t) + size);
    check_xmalloc(res);

    res = init_xmalloc(res, n * size);

    void *data = res;
    return data;
}

void xfree(void *data)
{
    if (data == NULL)
        return;
    struct xmalloc_t *xmalloc = get_header(data);

    if (xmalloc->next != NULL)
    {
        xmalloc->next->previous = xmalloc->previous;
    }

    if (xmalloc->previous == NULL)
    {
        head = xmalloc->next;
    }
    else
    {
        xmalloc->previous->next = xmalloc->next;
    }

    free(xmalloc);
}

void *xrealloc(void *data, size_t new_size)
{
    if (data == NULL)
        return xmalloc(new_size);
    struct xmalloc_t *xmalloc = get_header(data);
    xmalloc = realloc(xmalloc, sizeof(struct xmalloc_t) + new_size);
    check_xmalloc(xmalloc);

    if (xmalloc->previous != NULL)
        xmalloc->previous->next = xmalloc;
    else
        head = xmalloc;

    if (xmalloc->next != NULL)
        xmalloc->next->previous = xmalloc;

    xmalloc->allocation_size = new_size;

    data = xmalloc + 1;
    return data;
}

void *xrecalloc(void *data, size_t new_size)
{
    if (data == NULL)
        return xcalloc(1, new_size);
    size_t old_size = get_header(data)->allocation_size;
    data = xrealloc(data, new_size);

    char *start_data_addr = data;
    if (old_size < new_size)
        memset(start_data_addr + old_size, 0, new_size - old_size);

    return data;
}

void xfree_all(void)
{
    while (head != NULL)
    {
        xfree(head + 1);
    }
}
