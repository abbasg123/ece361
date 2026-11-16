#include "istack.h"

#include <stdio.h>

/* Takes an opening bracket and returns the closing bracket it requires.
 * Returns zero for any other character. Does not change a stack. */
static int closing_for(int opening)
{
    switch (opening)
    {
        case '(':
            return ')';
        case '[':
            return ']';
        case '{':
            return '}';
        default:
            return 0;
    }
}

/* Read stdin character by character. The stack holds unmatched openings;
 * its top is the opening that the next closing bracket must match.
 * Print one result line and return 0 if balanced, otherwise 1. */
int main(void)
{
    istack_t *s = istack_create();
    size_t line = 1;
    size_t column = 0;
    size_t pairs = 0;
    size_t deepest = 0;
    int result = 1;
    int ch;

    if (s == NULL)
    {
        fputs("error: out of memory\n", stderr);
        goto cleanup;
    }

    /* getchar returns int so that every input byte is distinct from EOF. */
    while ((ch = getchar()) != EOF)
    {
        column++;

        if (ch == '(' || ch == '[' || ch == '{')
        {
            if (istack_push(s, ch) != 0)
            {
                fputs("error: out of memory\n", stderr);
                goto cleanup;
            }

            size_t depth = istack_count(s);
            if (depth > deepest)
            {
                deepest = depth;
            }
        }
        else if (ch == ')' || ch == ']' || ch == '}')
        {
            int opening;
            if (istack_pop(s, &opening) != 0)
            {
                printf("line %zu, column %zu: found '%c', nothing open\n",
                       line, column, ch);
                goto cleanup;
            }

            int expected = closing_for(opening);
            if (ch != expected)
            {
                printf("line %zu, column %zu: found '%c', expected '%c'\n",
                       line, column, ch, expected);
                goto cleanup;
            }

            pairs++;
        }

        /* All characters count toward position, including ignored text.
         * Brackets in comments and quoted strings are processed normally. */
        if (ch == '\n')
        {
            line++;
            column = 0;
        }
    }

    if (ferror(stdin))
    {
        fputs("error: could not read input\n", stderr);
        goto cleanup;
    }

    if (!istack_is_empty(s))
    {
        int opening;
        istack_peek(s, &opening);
        printf("end of input: %zu still open, innermost '%c'\n",
               istack_count(s), opening);
        goto cleanup;
    }

    printf("balanced: %zu pairs, deepest %zu\n", pairs, deepest);
    result = 0;

cleanup:
    /* Every success and error path releases the same owned stack here. */
    istack_destroy(s);
    return result;
}
