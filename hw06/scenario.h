#ifndef SCENARIO_H
#define SCENARIO_H

#include <stddef.h>
#include <stdint.h>
#include <time.h>

#define SCENARIO_MAX_LINES 1000

/* Values the replay supplies to iom361 starting at tick. */
typedef struct
{
    int tick;           /* Nonnegative seconds since start. */
    float temp_c;
    float rh_pct;
    uint32_t switches;
} scenario_line_t;

typedef struct
{
    time_t start;       /* Unix timestamp from the start line. */
    size_t count;       /* Data rows, excluding comments and start. */
    scenario_line_t lines[SCENARIO_MAX_LINES];
} scenario_t;

/*
 * Load start <timestamp>, then tick temp rh hex-switches; # begins a comment.
 * Return 0, the first bad physical line number, or -1 for file/argument errors.
 * Missing required content is reported at EOF's line number. Failure keeps *s.
 */
int scenario_load(const char *filename, scenario_t *s);

/* Latest row at or before tick; NULL if none. The returned row belongs to s. */
const scenario_line_t *scenario_at(const scenario_t *s, int tick);

#endif
