#include "sensor.h"
#include "panel.h"
#include "scenario.h"
#include "iom361_r4.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s scenario.txt\n", argv[0]);
        return EXIT_FAILURE;
    }

    scenario_t scenario;
    int rc = scenario_load(argv[1], &scenario);
    if (rc < 0)
    {
        fprintf(stderr, "Cannot read scenario file: %s\n", argv[1]);
        return EXIT_FAILURE;
    }
    if (rc > 0)
    {
        fprintf(stderr, "%s: invalid or missing data at line %d\n", argv[1], rc);
        return EXIT_FAILURE;
    }

    /* Check the final timestamp before adding ticks to the start time. */
    int last_tick = scenario.lines[scenario.count - 1].tick;
    intmax_t start = (intmax_t)scenario.start;
    if (start > INTMAX_MAX - last_tick)
    {
        fprintf(stderr, "Scenario timestamps exceed the supported range\n");
        return EXIT_FAILURE;
    }
    intmax_t finish = start + last_tick;
    if ((intmax_t)(time_t)finish != finish)
    {
        fprintf(stderr, "Scenario timestamps do not fit time_t\n");
        return EXIT_FAILURE;
    }

    uint32_t *base = iom361_initialize(4, 4, &rc);
    if (base == NULL || rc != 0)
    {
        fprintf(stderr, "iom361 initialization failed: %d\n", rc);
        return EXIT_FAILURE;
    }
    iom361_set_display(false);

    uint32_t before = 0;
    for (int tick = 0; ; ++tick)
    {
        /* Keep initialized hardware values until the first scenario row. */
        const scenario_line_t *line = scenario_at(&scenario, tick);
        if (line != NULL)
        {
            _iom361_set_sensor1(line->temp_c, line->rh_pct);
            _iom361_set_switches(line->switches);
        }

        time_t now = (time_t)(start + tick);
        reading_t reading;
        rc = sensor_read(base, now, &reading);
        if (rc != 0)
        {
            fprintf(stderr, "Tick %d: sensor read failed: %d\n", tick, rc);
            return EXIT_FAILURE;
        }

        uint32_t switches = iom361_readReg(base, SWITCHES_REG, &rc);
        if (rc != 0)
        {
            fprintf(stderr, "Tick %d: switch read failed: %d\n", tick, rc);
            return EXIT_FAILURE;
        }

        /* A held switch must not toggle its LED again. */
        uint32_t pressed = panel_pressed(switches, before);
        for (int n = 0; n < 4; ++n)
        {
            if (pressed & (UINT32_C(1) << n))
            {
                rc = panel_toggle(base, n);
                if (rc != 0)
                {
                    fprintf(stderr, "Tick %d: LED %d toggle failed: %d\n", tick, n, rc);
                    return EXIT_FAILURE;
                }
            }
        }

        uint32_t leds = iom361_readReg(base, LEDS_REG, &rc);
        if (rc != 0)
        {
            fprintf(stderr, "Tick %d: LED read failed: %d\n", tick, rc);
            return EXIT_FAILURE;
        }

        char timestamp[32];
        struct tm *utc = gmtime(&reading.t);
        if (utc == NULL ||
            strftime(timestamp, sizeof timestamp, "%Y-%m-%d %H:%M:%S", utc) == 0)
        {
            fprintf(stderr, "Tick %d: cannot format the UTC timestamp\n", tick);
            return EXIT_FAILURE;
        }

        if (printf("%s temp=%.1f rh=%.1f switches=0x%" PRIX32
                   " pressed=0x%" PRIX32 " leds=0x%" PRIX32 "\n",
                   timestamp, reading.temp_c, reading.rh_pct, switches, pressed, leds) < 0)
        {
            fprintf(stderr, "Cannot write the replay trace\n");
            return EXIT_FAILURE;
        }

        before = switches;
        if (tick == last_tick)
        {
            break;
        }
    }

    return EXIT_SUCCESS;
}
