#include "headers/xmalloc.h"

#include <string.h>
#include <err.h>

static struct xmalloc_t *get_header(void *data)
{
    struct xmalloc_t *header = data;
    return --header;
}

static void check_xmalloc(struct xmalloc_t *res)
{
    if (res == NULL)
    {
        xfree_all();
        errx(1, "xmalloc : memory exhausted");
    }
}

static struct xmalloc_t *init_xmalloc(struct xmalloc_t *res)
{
    res->previous = NULL;
    res->next = head;

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

    res = init_xmalloc(res);

    void *data = res;
    return data;
}

void *xcalloc(size_t n, size_t size)
{
    size_t total_n = n + sizeof(struct xmalloc_t) / size + 1;
    struct xmalloc_t *res = calloc(total_n, sizeof(struct xmalloc_t) + size);
    check_xmalloc(res);

    res = init_xmalloc(res);

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

void *xrealloc(void* data, size_t new_size)
{
    struct xmalloc_t *xmalloc = get_header(data);
    xmalloc = realloc(xmalloc, sizeof(struct xmalloc_t) + new_size);
    check_xmalloc(xmalloc);

    if (xmalloc->previous != NULL)
        xmalloc->previous->next = xmalloc;

    if (xmalloc->next != NULL)
        xmalloc->next->previous = xmalloc;

    data = ++xmalloc;
    return data;
}

void xfree_all(void)
{
    while (head != NULL)
        xfree(++head);
}
