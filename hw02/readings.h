#ifndef READING_H
#define READING_H

#include <stdio.h>

#define MAX_READINGS 1000

/*
 * Read sensor data from input into three arrays.
 * Return the number of valid readings.
 * Store the number of malformed lines in *skipped.
 * Assume all arrays have room for MAX_READINGS elements.
 */
    int reading_read(FILE *input,
                    int ticks[],
                    float temps[],
                    float hums[],
                    int capacity,
                    int *skipped);

#endif
