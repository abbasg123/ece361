#ifndef STATS_H
#define STATS_H

/* Return the minimum of n array elements. Assumes n >= 1. */
float stats_min(const float a[], int n);

/*
 * Recursively returns the maximum of n elements.
 * Assumes n >= 1 and enough stack space for n calls.
 */
float stats_max(const float a[], int n);

/* Returns the arithmetic mean of n elements. Assumes n >= 1. */
float stats_mean(const float a[], int n);

/*
 * Returns the longest consecutive run strictly above threshold.
 * Stores the run's first index in startIndex, or -1 if no run.
 * Equal-length ties retain the first run.
 *
 * Assumes n >= 0, an array of n elements,
 * and a valid startIndex pointer.
 */
int stats_longest_run_above(const float a[], int n, float threshold,
                            int *startIndex);

#endif