#include "bst.h"
#include <stdio.h>

int main(void)
{
    bst_t *t = bst_create();
    if (t == NULL)
    {
        fputs("out of memory\n", stderr);
        return 1;
    }

    int key;
    int result;

    /* A successful conversion reads one integer; insert it before reading more. */
    while ((result = scanf("%d", &key)) == 1)
    {
        if (bst_insert(t, key) != 0)
        {
            fputs("out of memory\n", stderr);
            bst_destroy(t);
            return 1;
        }
    }

    if (ferror(stdin))
    {
        fputs("input error\n", stderr);
        bst_destroy(t);
        return 1;
    }

    /* Zero means input was present but could not be read as an integer. */
    if (result == 0)
    {
        printf("not a number after %zu values\n", bst_size(t));
        bst_destroy(t);
        return 1;
    }

    size_t n = bst_size(t);
    if (n == 0)
    {
        puts("empty tree");
    }
    else
    {
        printf("n %zu, height %zu\n", n, bst_height(t));
    }

    bst_destroy(t);
    return 0;
}
