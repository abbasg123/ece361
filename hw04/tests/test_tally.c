#include "../tally.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

static void check(const char *description, bool passed,
                  int *total, int *failures)
{
    (*total)++;

    if (passed)
    {
        printf("PASS: %s\n", description);
    }
    else
    {
        printf("FAIL: %s\n", description);
        (*failures)++;
    }
}

static bool printMatches(const tally_t *t, const char *expected)
{
    FILE *out = tmpfile();

    if (out == NULL)
    {
        return false;
    }

    tally_print(t, out);

    if (fflush(out) == EOF || fseek(out, 0, SEEK_SET) != 0)
    {
        fclose(out);
        return false;
    }

    bool matches = true;
    size_t position = 0;
    int ch;

    while ((ch = fgetc(out)) != EOF)
    {
        if (expected[position] == '\0' ||
            ch != (unsigned char)expected[position])
        {
            matches = false;
            break;
        }

        position++;
    }

    if (expected[position] != '\0' || ferror(out))
    {
        matches = false;
    }

    if (fclose(out) != 0)
    {
        matches = false;
    }

    return matches;
}

static int reportSummary(int total, int failures)
{
    printf("\nSummary: %d passed, %d failed, %d total\n",
           total - failures, failures, total);

    if (failures != 0)
    {
        return 1;
    }

    return 0;
}

int main(void)
{
    int total = 0;
    int failures = 0;

    tally_t *t = tally_create();
    check("create an empty tally", t != NULL, &total, &failures);

    if (t == NULL)
    {
        return reportSummary(total, failures);
    }

    check("new tally has size zero", tally_size(t) == 0,
          &total, &failures);
    check("empty tally prints no lines", printMatches(t, ""),
          &total, &failures);

    check("first occurrence returns 1", tally_add(t, "door") == 1,
          &total, &failures);
    check("second occurrence returns 2", tally_add(t, "door") == 2,
          &total, &failures);
    check("third occurrence returns 3", tally_add(t, "door") == 3,
          &total, &failures);
    check("count reports all three occurrences", tally_count(t, "door") == 3,
          &total, &failures);
    check("repeated name occupies only one entry", tally_size(t) == 1,
          &total, &failures);
    check("a name never added has count zero", tally_count(t, "motion") == 0,
          &total, &failures);

    check("empty name is refused", tally_add(t, "") == -1,
          &total, &failures);
    check("empty-name refusal leaves size and count unchanged",
          tally_size(t) == 1 && tally_count(t, "door") == 3,
          &total, &failures);


    char maxName[TALLY_LEN + 1];
    memset(maxName, 'a', TALLY_LEN);
    maxName[TALLY_LEN] = '\0';

    char tooLong[TALLY_LEN + 2];
    memset(tooLong, 'a', TALLY_LEN + 1);
    tooLong[TALLY_LEN + 1] = '\0';

    check("name of exactly TALLY_LEN characters is accepted",
          tally_add(t, maxName) == 1, &total, &failures);
    check("maximum-length name can be looked up",
          tally_count(t, maxName) == 1, &total, &failures);
    check("name of TALLY_LEN + 1 characters is refused",
          tally_add(t, tooLong) == -1, &total, &failures);
    check("overlong-name refusal leaves existing data unchanged",
          tally_size(t) == 2 && tally_count(t, "door") == 3 &&
          tally_count(t, maxName) == 1 && tally_count(t, tooLong) == 0,
          &total, &failures);

    check("add refuses a NULL tally", tally_add(NULL, "door") == -1,
          &total, &failures);
    check("add refuses a NULL name", tally_add(t, NULL) == -1,
          &total, &failures);
    check("add refuses two NULL arguments", tally_add(NULL, NULL) == -1,
          &total, &failures);
    check("count returns zero for a NULL tally", tally_count(NULL, "door") == 0,
          &total, &failures);
    check("count returns zero for a NULL name", tally_count(t, NULL) == 0,
          &total, &failures);
    check("count returns zero for two NULL arguments", tally_count(NULL, NULL) == 0,
          &total, &failures);
    check("size returns zero for NULL", tally_size(NULL) == 0,
          &total, &failures);

    tally_destroy(NULL);
    check("destroy(NULL) and refused NULL additions leave live data unchanged",
          tally_size(t) == 2 && tally_count(t, "door") == 3 &&
          tally_count(t, maxName) == 1,
          &total, &failures);

    char callerName[] = "sensor";
    check("accept a name from a writable array", tally_add(t, callerName) == 1,
          &total, &failures);
    callerName[0] = 'X';
    check("tally owns a separate copy of the name",
          tally_count(t, "sensor") == 1 && tally_count(t, callerName) == 0,
          &total, &failures);

    tally_destroy(t);
    t = tally_create();
    check("create succeeds after destroy", t != NULL, &total, &failures);

    if (t == NULL)
    {
        return reportSummary(total, failures);
    }

    check("recreated tally starts empty with no old names",
          tally_size(t) == 0 && tally_count(t, "door") == 0 &&
          tally_count(t, maxName) == 0 && tally_count(t, "sensor") == 0,
          &total, &failures);

    const char *printNames[] =
    {
        "window", "door", "motion", "light", "alarm", "motion"
    };
    const int printCounts[] = {1, 1, 1, 1, 1, 2};
    bool printSetup = true;

    for (int i = 0; i < 6; i++)
    {
        if (tally_add(t, printNames[i]) != printCounts[i])
        {
            printSetup = false;
        }
    }

    check("out-of-order additions return the expected counts", printSetup,
          &total, &failures);
    check("print uses strcmp order and the correct name-count format",
          printMatches(t, "alarm 1\ndoor 1\nlight 1\nmotion 2\nwindow 1\n"),
          &total, &failures);
    check("printing leaves stored names and counts unchanged",
          tally_size(t) == 5 && tally_count(t, "alarm") == 1 &&
          tally_count(t, "door") == 1 && tally_count(t, "light") == 1 &&
          tally_count(t, "motion") == 2 && tally_count(t, "window") == 1,
          &total, &failures);

    tally_destroy(t);
    t = tally_create();
    check("create a tally for the capacity tests", t != NULL,
          &total, &failures);

    if (t == NULL)
    {
        return reportSummary(total, failures);
    }

    bool allAdded = true;

    for (int i = TALLY_NAMES - 1; i >= 0; i--)
    {
        char name[TALLY_LEN + 1];
        snprintf(name, sizeof(name), "name%02d", i);

        if (tally_add(t, name) != 1)
        {
            allAdded = false;
        }
    }

    check("all TALLY_NAMES different names fit",
          allAdded && tally_size(t) == TALLY_NAMES, &total, &failures);
    check("a full tally refuses a new name", tally_add(t, "extra") == -1,
          &total, &failures);

    bool unchanged = tally_size(t) == TALLY_NAMES && tally_count(t, "extra") == 0;

    for (int i = 0; i < TALLY_NAMES; i++)
    {
        char name[TALLY_LEN + 1];
        snprintf(name, sizeof(name), "name%02d", i);

        if (tally_count(t, name) != 1)
        {
            unchanged = false;
        }
    }

    check("full-tally refusal preserves every existing name and count",
          unchanged, &total, &failures);
    check("a full tally still increments an existing name",
          tally_add(t, "name07") == 2, &total, &failures);
    check("counting an existing name keeps a full tally's size unchanged",
          tally_size(t) == TALLY_NAMES && tally_count(t, "name07") == 2,
          &total, &failures);
    tally_destroy(t);

    tally_t *pool[TALLY_MAX] = {NULL};
    bool allCreated = true;
    bool allDistinct = true;

    for (int i = 0; i < TALLY_MAX; i++)
    {
        pool[i] = tally_create();

        if (pool[i] == NULL)
        {
            allCreated = false;
        }

        for (int j = 0; j < i; j++)
        {
            if (pool[i] != NULL && pool[i] == pool[j])
            {
                allDistinct = false;
            }
        }
    }

    check("TALLY_MAX tallies can exist at the same time", allCreated,
          &total, &failures);
    check("simultaneous creations return different tallies",
          allCreated && allDistinct, &total, &failures);

    tally_t *extra = tally_create();
    check("one creation beyond TALLY_MAX returns NULL", extra == NULL,
          &total, &failures);

    if (extra != NULL)
    {
        tally_destroy(extra);
    }

    bool independent = false;

    if (pool[0] != NULL && pool[1] != NULL && pool[0] != pool[1])
    {
        int firstAdd = tally_add(pool[0], "door");
        int secondAdd = tally_add(pool[1], "motion");

        independent = firstAdd == 1 && secondAdd == 1 &&
                      tally_count(pool[0], "door") == 1 &&
                      tally_count(pool[0], "motion") == 0 &&
                      tally_count(pool[1], "motion") == 1 &&
                      tally_count(pool[1], "door") == 0 &&
                      tally_size(pool[0]) == 1 && tally_size(pool[1]) == 1;
    }

    check("two live tallies store independent names and counts", independent,
          &total, &failures);

    tally_destroy(pool[0]);
    pool[0] = tally_create();
    check("destroy frees a slot even when the pool was full", pool[0] != NULL,
          &total, &failures);
    check("the replacement tally is empty",
          pool[0] != NULL && tally_size(pool[0]) == 0 &&
          tally_count(pool[0], "door") == 0,
          &total, &failures);
    check("releasing one tally leaves another tally's data intact",
          pool[1] != NULL && tally_count(pool[1], "motion") == 1 &&
          tally_size(pool[1]) == 1,
          &total, &failures);

    for (int i = 0; i < TALLY_MAX; i++)
    {
        tally_destroy(pool[i]);
    }

    return reportSummary(total, failures);
}
