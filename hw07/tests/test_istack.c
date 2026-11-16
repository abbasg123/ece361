#include "istack.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

static size_t tests_run;
static size_t tests_failed;

/* Record one test result; main uses the failure count as its exit status. */
static void report(const char *name, bool passed)
{
    tests_run++;
    if (!passed)
    {
        tests_failed++;
    }
    printf("%s: %s\n", passed ? "PASS" : "FAIL", name);
}

static void test_create(void)
{
    istack_t *s = istack_create();
    report("new stack is empty with capacity 8",
           s != NULL && istack_count(s) == 0 && istack_is_empty(s) &&
           istack_capacity(s) == ISTACK_START);
    istack_destroy(s);
}

static void test_empty(void)
{
    istack_t *s = istack_create();
    if (s == NULL)
    {
        report("empty stack tests: allocation", false);
        return;
    }

    int value = 12345;
    report("empty pop leaves output unchanged",
           istack_pop(s, &value) == -1 && value == 12345 &&
           istack_count(s) == 0);
    report("empty peek leaves output unchanged",
           istack_peek(s, &value) == -1 && value == 12345 &&
           istack_count(s) == 0);
    report("empty pop and peek allow NULL output",
           istack_pop(s, NULL) == -1 && istack_peek(s, NULL) == -1);

    size_t n = 99;
    int *copy = istack_to_array(s, &n);
    report("empty snapshot returns NULL and count zero", copy == NULL && n == 0);
    free(copy);
    istack_destroy(s);
}

static void test_lifo(void)
{
    istack_t *s = istack_create();
    if (s == NULL)
    {
        report("LIFO tests: allocation", false);
        return;
    }

    const int values[] = {INT_MIN, -1, 0, 20, INT_MAX};
    const size_t n = sizeof values / sizeof values[0];
    bool passed = true;
    for (size_t i = 0; i < n; i++)
    {
        if (istack_push(s, values[i]) != 0)
        {
            passed = false;
        }
    }

    int value = 0;
    report("peek sees top without removing it",
           passed && istack_peek(s, &value) == 0 && value == INT_MAX &&
           istack_count(s) == n);

    for (size_t i = n; i > 0; i--)
    {
        if (istack_pop(s, &value) != 0 || value != values[i - 1])
        {
            passed = false;
        }
    }
    report("LIFO includes zero, negative values, and int limits",
           passed && istack_is_empty(s));
    istack_destroy(s);
}

static void test_growth(void)
{
    istack_t *s = istack_create();
    if (s == NULL)
    {
        report("growth tests: allocation", false);
        return;
    }

    bool passed = true;
    for (int i = 0; i < ISTACK_START; i++)
    {
        if (istack_push(s, 100 + i) != 0)
        {
            passed = false;
        }
    }
    report("eight values fill capacity 8 without growing",
           passed && istack_count(s) == 8 && istack_capacity(s) == 8);
    report("ninth push doubles capacity to 16",
           istack_push(s, 108) == 0 && istack_count(s) == 9 &&
           istack_capacity(s) == 16);

    for (int i = 8; i >= 0; i--)
    {
        int value = 0;
        if (istack_pop(s, &value) != 0 || value != 100 + i)
        {
            passed = false;
        }
    }
    report("growth preserves every value and pop never shrinks capacity",
           passed && istack_is_empty(s) && istack_capacity(s) == 16);
    istack_destroy(s);
}

static void test_thousand(void)
{
    istack_t *s = istack_create();
    if (s == NULL)
    {
        report("1000 values: allocation", false);
        return;
    }

    bool passed = true;
    for (int i = 0; i < 1000; i++)
    {
        if (istack_push(s, i) != 0)
        {
            passed = false;
        }
    }
    report("1000 pushes repeatedly double capacity to 1024",
           passed && istack_count(s) == 1000 && istack_capacity(s) == 1024);

    for (int i = 999; i >= 0; i--)
    {
        int value = 0;
        if (istack_pop(s, &value) != 0 || value != i)
        {
            passed = false;
        }
    }
    report("1000 pops return every value in reverse order",
           passed && istack_is_empty(s) && istack_capacity(s) == 1024);
    istack_destroy(s);
}

static void test_snapshot(void)
{
    istack_t *s = istack_create();
    if (s == NULL)
    {
        report("snapshot tests: allocation", false);
        return;
    }

    bool pushed = istack_push(s, 10) == 0 && istack_push(s, 20) == 0 &&
                  istack_push(s, 30) == 0;
    size_t n = 0;
    int *copy = istack_to_array(s, &n);
    bool valid = pushed && copy != NULL && n == 3;
    report("snapshot is bottom first and leaves stack unchanged",
           valid && copy[0] == 10 && copy[1] == 20 && copy[2] == 30 &&
           istack_count(s) == 3);

    bool independent = false;
    if (valid)
    {
        copy[2] = 999;
        int value = 0;
        independent = istack_peek(s, &value) == 0 && value == 30;
    }
    report("changing snapshot does not change the stack", independent);

    istack_destroy(s);
    report("snapshot remains owned by caller after stack destruction",
           valid && copy[0] == 10 && copy[2] == 999);
    free(copy);
}

static void test_null(void)
{
    int value = 777;
    report("NULL push fails", istack_push(NULL, 5) == -1);
    report("NULL pop and peek leave output unchanged",
           istack_pop(NULL, &value) == -1 && value == 777 &&
           istack_peek(NULL, &value) == -1 && value == 777);
    report("NULL stack with NULL value outputs is safe",
           istack_pop(NULL, NULL) == -1 && istack_peek(NULL, NULL) == -1);
    report("NULL count, empty, and capacity results",
           istack_count(NULL) == 0 && istack_is_empty(NULL) &&
           istack_capacity(NULL) == 0);

    size_t n = 777;
    int *copy = istack_to_array(NULL, &n);
    report("NULL snapshot resets output length", copy == NULL && n == 0);
    free(copy);
    copy = istack_to_array(NULL, NULL);
    report("snapshot accepts both arguments NULL", copy == NULL);
    free(copy);
    istack_destroy(NULL);
    report("destroy NULL does nothing", true);
}

static void test_optional_outputs(void)
{
    istack_t *s = istack_create();
    if (s == NULL)
    {
        report("optional outputs: allocation", false);
        return;
    }

    bool pushed = istack_push(s, 42) == 0;
    report("NULL peek output preserves the value",
           pushed && istack_peek(s, NULL) == 0 && istack_count(s) == 1);
    int *copy = istack_to_array(s, NULL);
    report("snapshot allows omitted length output", copy != NULL && copy[0] == 42);
    free(copy);
    report("NULL pop output still removes the value",
           istack_pop(s, NULL) == 0 && istack_is_empty(s));
    istack_destroy(s);
}

static void test_independent(void)
{
    istack_t *a = istack_create();
    istack_t *b = istack_create();
    if (a == NULL || b == NULL)
    {
        report("independent stacks: allocation", false);
        istack_destroy(a);
        istack_destroy(b);
        return;
    }

    bool passed = istack_push(b, -99) == 0;
    for (int i = 0; i < 100; i++)
    {
        if (istack_push(a, i) != 0)
        {
            passed = false;
        }
    }
    int value = 0;
    report("growing one stack does not change another",
           passed && istack_count(a) == 100 && istack_count(b) == 1 &&
           istack_capacity(b) == 8 && istack_peek(b, &value) == 0 && value == -99);
    istack_destroy(a);
    report("destroying one stack leaves another usable",
           istack_pop(b, &value) == 0 && value == -99 && istack_is_empty(b));
    istack_destroy(b);
}

static void test_destroy_nonempty(void)
{
    istack_t *s = istack_create();
    if (s == NULL)
    {
        report("destroy nonempty: allocation", false);
        return;
    }

    bool passed = true;
    for (int i = 0; i < 20; i++)
    {
        if (istack_push(s, i) != 0)
        {
            passed = false;
        }
    }
    /* Sanitizers verify that destruction frees both allocations safely. */
    istack_destroy(s);
    report("destroy a nonempty grown stack", passed);
}

int main(void)
{
    test_create();
    test_empty();
    test_lifo();
    test_growth();
    test_thousand();
    test_snapshot();
    test_null();
    test_optional_outputs();
    test_independent();
    test_destroy_nonempty();

    printf("%zu tests: %zu passed, %zu failed\n",
           tests_run, tests_run - tests_failed, tests_failed);
    return tests_failed == 0 ? 0 : 1;
}
