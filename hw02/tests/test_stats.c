#include <stdio.h>
#include "../stats.h"

/* Print one test result and return 1 for a pass, 0 for a failure. */
static int reportTest(const char *name, int passed)
{
    printf("%s %s\n", passed ? "PASS" : "FAIL", name);
    return passed;
}

int main(void)
{
    int passed = 0;
    int total = 0;
    int startIndex;
    int run;

    float one[] = {12.5f};
    float maxAtStart[] = {9.0f, 2.0f, -3.0f, 5.0f};
    float maxAtEnd[] = {2.0f, -3.0f, 5.0f, 9.0f};
    float equalThreshold[] = {31.0f, 32.0f, 30.0f, 33.0f, 34.0f, 35.0f};
    float large[10000];
    float negative[] = {-4.0f, -2.0f, -8.0f, -6.0f};
    float noRun[] = {28.0f, 30.0f, 29.0f};
    float tiedRuns[] = {31.0f, 32.0f, 30.0f, 33.0f, 34.0f};

    passed += reportTest("one value",
                         stats_min(one, 1) == 12.5f &&
                         stats_max(one, 1) == 12.5f &&
                         stats_mean(one, 1) == 12.5f);
    total++;

    passed += reportTest("maximum at start",
                         stats_max(maxAtStart, 4) == 9.0f &&
                         stats_min(maxAtStart, 4) == -3.0f &&
                         stats_mean(maxAtStart, 4) == 3.25f);
    total++;

    passed += reportTest("maximum at end",
                         stats_max(maxAtEnd, 4) == 9.0f);
    total++;

    run = stats_longest_run_above(equalThreshold, 6, 30.0f, &startIndex);
    passed += reportTest("equal to threshold ends a run",
                         run == 3 && startIndex == 3);
    total++;

    for (int i = 0; i < 10000; i++)
    {
        large[i] = (float)i;
    }
    large[4321] = 12345.0f;

    passed += reportTest("recursive maximum on 10000 values",
                         stats_max(large, 10000) == 12345.0f);
    total++;

    passed += reportTest("negative values",
                         stats_min(negative, 4) == -8.0f &&
                         stats_max(negative, 4) == -2.0f &&
                         stats_mean(negative, 4) == -5.0f);
    total++;

    startIndex = 99;
    run = stats_longest_run_above(noRun, 3, 30.0f, &startIndex);
    passed += reportTest("no readings above threshold",
                         run == 0 && startIndex == -1);
    total++;

    run = stats_longest_run_above(tiedRuns, 5, 30.0f, &startIndex);
    passed += reportTest("equal-length runs keep the first",
                         run == 2 && startIndex == 0);
    total++;

    printf("Summary: %d passed, %d failed, %d total\n",
           passed, total - passed, total);

    if (passed != total)
    {
        return 1;
    }

    return 0;
}
