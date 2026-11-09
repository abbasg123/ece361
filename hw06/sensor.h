#ifndef SENSOR_H
#define SENSOR_H

#include <stdint.h>
#include <time.h>

/* One sensor sample: when it was taken and its converted measurements. */
typedef struct
{
    time_t t;       /* Scenario start + tick, in Unix seconds (UTC). */
    float temp_c;   /* Temperature in degrees Celsius, rounded to 0.1. */
    float rh_pct;   /* Relative humidity in percent, rounded to 0.1. */
} reading_t;

/*
 * Read one temperature/humidity sample for the caller's trace or records.
 *
 * base: register-block address returned by iom361_initialize().
 * now: caller's simulated timestamp; the sensor module keeps no clock.
 * r: non-NULL pointer to the reading that will receive the complete sample.
 *
 * Read TEMP_REG and HUMID_REG, keeping only bits 19..0 of each:
 *   temp_c = (raw_temp / 1048576.0) * 200.0 - 50.0
 *   rh_pct = (raw_rh   / 1048576.0) * 100.0
 * Round both to the nearest 0.1 so raw-register rounding does not make an
 * exact threshold reading, such as 57.0 C, appear just below that threshold.
 *
 * Return 0 after storing now and both measurements in *r. If either register
 * read fails, return its iom361 error code and leave *r entirely unchanged.
 * This module does not print or call the _iom361_set_... test functions.
 */
int sensor_read(uint32_t *base, time_t now, reading_t *r);

#endif
