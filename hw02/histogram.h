#ifndef HISTOGRAM_H
#define HISTOGRAM_H

/*
 * Print histogram of n temperatures to standard output.
 * Return nothing
 *
 * Assumong  n >= 0 and an array containing n finite temperatures
 * whose value divided by 5 fits in an int.
 * Uses twenty 5 C bins, placing out-of-range values in the end bins.
 */
void histogram_print(const float temps[], int n);

#endif