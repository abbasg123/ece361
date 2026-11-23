#include "bst.h"
#include <stdio.h>

/* Return one for a failed check so main can count failures. */
static int check(bool passed, const char *name)
{
    if (passed)
    {
        printf("PASS: %s\n", name);
        return 0;
    }

    printf("FAIL: %s\n", name);
    return 1;
}

static int test_empty_tree(void)
{
    bst_t *t = bst_create();
    if (t == NULL)
    {
        return check(false, "create an empty tree");
    }

    int failures = 0;
    failures += check(bst_size(t) == 0, "empty tree has size 0");
    failures += check(bst_height(t) == 0, "empty tree has height 0");

    /* Flush the test name so it remains visible if the original search crashes. */
    puts("RUN: search in an empty tree (original crash case)");
    fflush(stdout);

    size_t visits = 99;
    bool found = bst_contains(t, 27, &visits);
    failures += check(!found && visits == 0,
                      "empty-tree search returns false with 0 visits");

    bst_destroy(t);
    return failures;
}

static int test_original_tree(void)
{
    int keys[] =
    {
        27, 14, 35, 10, 19, 31, 42
    };
    bst_t *t = bst_create();
    if (t == NULL)
    {
        return check(false, "create the original seven-key tree");
    }

    for (size_t i = 0; i < sizeof keys / sizeof keys[0]; i++)
    {
        if (bst_insert(t, keys[i]) != 0)
        {
            bst_destroy(t);
            return check(false, "insert the original seven keys");
        }
    }

    int failures = 0;
    failures += check(bst_size(t) == 7, "original tree has size 7");
    failures += check(bst_height(t) == 3, "original tree has height 3");

    size_t visits = 99;
    bool found = bst_contains(t, 31, &visits);
    failures += check(found && visits == 3, "31 is found in 3 visits");

    visits = 99;
    found = bst_contains(t, 15, &visits);
    failures += check(!found && visits == 3, "15 is not found in 3 visits");

    visits = 99;
    found = bst_contains(t, 27, &visits);
    failures += check(found && visits == 1, "root is found in 1 visit");

    failures += check(bst_contains(t, 31, NULL),
                      "successful search accepts NULL visits");
    failures += check(!bst_contains(t, 15, NULL),
                      "unsuccessful search accepts NULL visits");

    /* Memory checking detects leaks or invalid frees from this destruction. */
    bst_destroy(t);
    puts("PASS: destroying a full tree returns normally");
    return failures;
}

static int test_duplicates(void)
{
    bst_t *t = bst_create();
    if (t == NULL)
    {
        return check(false, "create the duplicate-key tree");
    }

    if (bst_insert(t, 30) != 0 || bst_insert(t, 30) != 0)
    {
        bst_destroy(t);
        return check(false, "insert two copies of 30");
    }

    int failures = 0;
    failures += check(bst_size(t) == 2, "duplicate key is kept: size 2");
    failures += check(bst_height(t) == 2, "two equal keys occupy two nodes");

    size_t visits = 99;
    bool found = bst_contains(t, 30, &visits);
    failures += check(found && visits == 1,
                      "duplicate search stops at the first matching node");

    if (bst_insert(t, 31) != 0)
    {
        bst_destroy(t);
        return failures + check(false, "insert 31 after the duplicates");
    }

    /* Right-hand duplicates make 30, 30, 31 a three-node path. */
    failures += check(bst_height(t) == 3, "equal keys take the right branch");

    bst_destroy(t);
    return failures;
}

static int test_increasing_order(void)
{
    bst_t *t = bst_create();
    if (t == NULL)
    {
        return check(false, "create the increasing-order tree");
    }

    for (int key = 1; key <= 32; key++)
    {
        if (bst_insert(t, key) != 0)
        {
            bst_destroy(t);
            return check(false, "insert keys 1 through 32");
        }
    }

    int failures = 0;
    failures += check(bst_size(t) == 32, "32 increasing keys give size 32");
    failures += check(bst_height(t) == 32, "32 increasing keys give height 32");

    size_t visits = 99;
    bool found = bst_contains(t, 32, &visits);
    failures += check(found && visits == 32,
                      "last increasing key takes 32 visits");

    bst_destroy(t);
    return failures;
}

static int test_null_tree(void)
{
    int failures = 0;
    failures += check(bst_insert(NULL, 10) == -1, "inserting into NULL fails");
    failures += check(bst_size(NULL) == 0, "NULL tree has size 0");
    failures += check(bst_height(NULL) == 0, "NULL tree has height 0");

    size_t visits = 99;
    bool found = bst_contains(NULL, 10, &visits);
    failures += check(!found && visits == 0,
                      "NULL-tree search returns false with 0 visits");
    failures += check(!bst_contains(NULL, 10, NULL),
                      "NULL-tree search also accepts NULL visits");

    bst_destroy(NULL);
    puts("PASS: destroying NULL returns normally");
    return failures;
}

int main(void)
{
    int failures = 0;

    /* Keep this first: the initial search port must expose its empty-tree bug. */
    failures += test_empty_tree();
    failures += test_original_tree();
    failures += test_duplicates();
    failures += test_increasing_order();
    failures += test_null_tree();

    if (failures != 0)
    {
        printf("%d check(s) failed.\n", failures);
        return 1;
    }

    puts("All tests passed.");
    return 0;
}
