#include "../ptr.h"
#include <stddef.h>
#include <stdio.h>

static int testsPassed = 0;
static int testsFailed = 0;

static void check(int condition, const char *name)
{
    if(condition)
    {
        printf("PASS: %s\n", name);
        testsPassed++;
    }
    else
    {
        printf("FAIL: %s\n", name);
        testsFailed++;
    }
}

static int arraysEqual(const float *a, const float *b, int n)
{
    for(int i = 0; i < n; i++)
    {
        if(a[i] != b[i])
        {
            return 0;
        }
    }
    return 1;
}

static void testSwapFloat(void)
{
    float x = 3.0f;
    float y = -7.0f;
    swapFloat(&x, &y);
    check(x == -7.0f && y == 3.0f,
          "swapFloat exchanges two different values");

    x = 4.0f;
    y = 4.0f;
    swapFloat(&x, &y);
    check(x == 4.0f && y == 4.0f,
          "swapFloat handles equal values");

    x = 6.0f;
    swapFloat(&x, &x);
    check(x == 6.0f,
          "swapFloat handles two pointers to the same float");
}

static void testMinMax(void)
{
    const float values[] = {4.0f, -2.0f, 7.0f, 9.0f};
    const float positive[] = {8.0f, 6.0f, 3.0f, 2.0f};
    const float negative[] = {-4.0f, -8.0f, -2.0f};
    const float single[] = {5.0f};
    float min = 123.0f;
    float max = -456.0f;
    int rc;

    rc = minMax(values, 4, &min, &max);
    check(rc == 0 && min == -2.0f && max == 9.0f,
          "minMax finds both results in several values");

    rc = minMax(positive, 4, &min, &max);
    check(rc == 0 && min == 2.0f && max == 8.0f,
          "minMax handles positive values and a minimum at the end");

    rc = minMax(negative, 3, &min, &max);
    check(rc == 0 && min == -8.0f && max == -2.0f,
          "minMax handles entirely negative values");

    rc = minMax(single, 1, &min, &max);
    check(rc == 0 && min == 5.0f && max == 5.0f,
          "minMax handles one value");

    min = 123.0f;
    max = -456.0f;
    rc = minMax(values, 0, &min, &max);
    check(rc == -1 && min == 123.0f && max == -456.0f,
          "minMax rejects n = 0 without changing either output");

    min = 123.0f;
    max = -456.0f;
    rc = minMax(values, -1, &min, &max);
    check(rc == -1 && min == 123.0f && max == -456.0f,
          "minMax rejects a negative length without changing either output");

    min = 123.0f;
    max = -456.0f;
    rc = minMax(NULL, 4, &min, &max);
    check(rc == -1 && min == 123.0f && max == -456.0f,
          "minMax rejects a NULL array without changing either output");

    min = 123.0f;
    max = -456.0f;
    rc = minMax(values, 4, NULL, &max);
    check(rc == -1 && max == -456.0f,
          "minMax rejects a NULL min pointer without changing max");

    min = 123.0f;
    max = -456.0f;
    rc = minMax(values, 4, &min, NULL);
    check(rc == -1 && min == 123.0f,
          "minMax rejects a NULL max pointer without changing min");
}

static void testFindAbove(void)
{
    const float twoMatches[] = {1.0f, 5.0f, 7.0f, 2.0f};
    const float firstMatch[] = {6.0f, 2.0f, 9.0f};
    const float noMatch[] = {1.0f, 2.0f, 3.0f};
    const float equalOnly[] = {1.0f, 4.0f, 2.0f};
    const float equalThenMatch[] = {4.0f, 6.0f, 7.0f};
    const float lastMatch[] = {1.0f, 2.0f, 8.0f};

    check(findAbove(twoMatches, 4, 4.0f) == twoMatches + 1,
          "findAbove returns the first matching address when two values match");

    check(findAbove(firstMatch, 3, 5.0f) == firstMatch,
          "findAbove can return the first element's address");

    check(findAbove(noMatch, 3, 4.0f) == NULL,
          "findAbove returns NULL when no value exceeds the threshold");

    check(findAbove(equalOnly, 3, 4.0f) == NULL,
          "findAbove does not count equality as a match");

    check(findAbove(equalThenMatch, 3, 4.0f) == equalThenMatch + 1,
          "findAbove skips equality and returns the next qualifying address");

    check(findAbove(lastMatch, 3, 5.0f) == lastMatch + 2,
          "findAbove can return the last element's address");

    check(findAbove(twoMatches, 0, 4.0f) == NULL,
          "findAbove returns NULL for n = 0");

    check(findAbove(twoMatches, -1, 4.0f) == NULL,
          "findAbove returns NULL for a negative length");

    check(findAbove(NULL, 4, 4.0f) == NULL,
          "findAbove returns NULL for a NULL array");
}

static void testReverse(void)
{
    float even[] = {1.0f, 2.0f, 3.0f, 4.0f};
    const float expectedEven[] = {4.0f, 3.0f, 2.0f, 1.0f};
    reverse(even, 4);
    check(arraysEqual(even, expectedEven, 4),
          "reverse reverses an even-length array");

    float odd[] = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f};
    const float expectedOdd[] = {5.0f, 4.0f, 3.0f, 2.0f, 1.0f};
    reverse(odd, 5);
    check(arraysEqual(odd, expectedOdd, 5),
          "reverse reverses an odd-length array");

    float pair[] = {-1.0f, 8.0f};
    const float expectedPair[] = {8.0f, -1.0f};
    reverse(pair, 2);
    check(arraysEqual(pair, expectedPair, 2),
          "reverse reverses two adjacent elements");

    float single[] = {5.0f};
    reverse(single, 1);
    check(single[0] == 5.0f,
          "reverse leaves a one-element array unchanged");

    float unchanged[] = {2.0f, 4.0f};
    const float expectedUnchanged[] = {2.0f, 4.0f};
    reverse(unchanged, 0);
    check(arraysEqual(unchanged, expectedUnchanged, 2),
          "reverse leaves the array unchanged for n = 0");

    reverse(unchanged, -1);
    check(arraysEqual(unchanged, expectedUnchanged, 2),
          "reverse leaves the array unchanged for a negative length");

    reverse(NULL, 4);
    /* Returning safely is the observable result of this no-op call. */
    check(1, "reverse returns safely for a NULL array");
}

int main(void)
{
    testSwapFloat();
    testMinMax();
    testFindAbove();
    testReverse();

    printf("\nSummary: %d passed, %d failed\n", testsPassed, testsFailed);
    return testsFailed != 0;
}
