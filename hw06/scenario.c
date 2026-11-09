#include "scenario.h"
#include <errno.h>
#include <inttypes.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* strto... functions detect overflow that numeric sscanf conversions cannot. */
static int parse_integer(const char *text, intmax_t *value)
{
    char *end;
    errno = 0;
    intmax_t number = strtoimax(text, &end, 10);
    if (end == text || *end != '\0' || errno == ERANGE)
    {
        return 0;
    }

    *value = number;
    return 1;
}

static int parse_measurement(const char *text, float *value)
{
    char *end;
    errno = 0;
    float number = strtof(text, &end);
    if (end == text || *end != '\0' || errno == ERANGE || !isfinite(number))
    {
        return 0;
    }

    *value = number;
    return 1;
}

static int parse_switches(const char *text, uint32_t *value)
{
    if (*text == '-')
    {
        return 0;
    }

    char *end;
    errno = 0;
    uintmax_t number = strtoumax(text, &end, 16);
    if (end == text || *end != '\0' || errno == ERANGE || number > UINT32_MAX)
    {
        return 0;
    }

    *value = (uint32_t)number;
    return 1;
}

int scenario_load(const char *filename, scenario_t *s)
{
    if (filename == NULL || s == NULL)
    {
        return -1;
    }

    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        return -1;
    }

    scenario_t loaded = {0};
    char buffer[1024];
    int line_number = 0;
    int have_start = 0;
    int status = 0;

    while (fgets(buffer, sizeof buffer, file) != NULL)
    {
        ++line_number;

        /* Never accept a truncated row as a complete input line. */
        if (strchr(buffer, '\n') == NULL && !feof(file))
        {
            status = line_number;
            break;
        }

        char *comment = strchr(buffer, '#');
        if (comment != NULL)
        {
            *comment = '\0';
        }

        /* A fifth field detects unwanted trailing input. */
        char *fields[5];
        size_t count = 0;
        char *field = strtok(buffer, " \t\r\n\v\f");
        while (field != NULL && count < 5)
        {
            fields[count++] = field;
            field = strtok(NULL, " \t\r\n\v\f");
        }

        if (count == 0)
        {
            continue;
        }

        if (!have_start)
        {
            intmax_t timestamp;
            if (count != 2 || strcmp(fields[0], "start") != 0 ||
                !parse_integer(fields[1], &timestamp))
            {
                status = line_number;
                break;
            }

            loaded.start = (time_t)timestamp;
            if ((intmax_t)loaded.start != timestamp ||
                (timestamp < 0 && loaded.start > (time_t)0))
            {
                status = line_number;
                break;
            }

            have_start = 1;
            continue;
        }

        scenario_line_t row;
        intmax_t tick;
        if (count != 4 || loaded.count == SCENARIO_MAX_LINES ||
            !parse_integer(fields[0], &tick) || tick < 0 || tick > INT_MAX ||
            !parse_measurement(fields[1], &row.temp_c) ||
            !parse_measurement(fields[2], &row.rh_pct) ||
            !parse_switches(fields[3], &row.switches))
        {
            status = line_number;
            break;
        }

        if (loaded.count > 0 && tick <= loaded.lines[loaded.count - 1].tick)
        {
            status = line_number;
            break;
        }

        row.tick = (int)tick;
        loaded.lines[loaded.count++] = row;
    }

    if (status == 0 && ferror(file))
    {
        status = -1;
    }
    if (status == 0 && (!have_start || loaded.count == 0))
    {
        status = line_number + 1;
    }
    if (fclose(file) != 0 && status == 0)
    {
        status = -1;
    }

    /* A malformed file must not replace the caller's previous scenario. */
    if (status == 0)
    {
        *s = loaded;
    }
    return status;
}

const scenario_line_t *scenario_at(const scenario_t *s, int tick)
{
    if (s == NULL || s->count == 0 || tick < s->lines[0].tick)
    {
        return NULL;
    }

    size_t next = 1;
    while (next < s->count && s->lines[next].tick <= tick)
    {
        ++next;
    }

    return &s->lines[next - 1];
}
