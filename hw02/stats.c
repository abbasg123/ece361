#include "stats.h"

float stats_min(const float a[], int n)
{
    float minimum = a[0];

    for (int i = 1; i < n; i++)
    {
        if (a[i] < minimum)
        {
            minimum = a[i];
        }
    }

    return minimum;
}

float stats_max(const float a[], int n)
{
    if (n == 1)
    {
        return a[0];
    }

    float rest = stats_max(a + 1, n - 1);

    return a[0] > rest ? a[0] : rest;
}

float stats_mean(const float a[], int n)
{
    float sum = 0.0f;

    for (int i = 0; i < n; i++)
    {
        sum += a[i];
    }

    return sum / n;
}

int stats_longest_run_above(const float a[], int n, float threshold,
                            int *startIndex)
{
    int run = 0;
    int best = 0;
    int start = 0;
    *startIndex = -1;

    for (int i = 0; i < n; i++)
    {
        if (a[i] > threshold)
        {
            if (run == 0)
            {
                start = i;
            }

            run++;

            if (run > best)
            {
                best = run;
                *startIndex = start;
            }
        }
        else
        {
            run = 0;
        }
    }

    return best;
}