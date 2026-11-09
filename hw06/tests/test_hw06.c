#include "sensor.h"
#include "panel.h"
#include "scenario.h"
#include "iom361_r4.h"
#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int passed;
static int failed;

static void check(int condition, const char *name)
{
    printf("%s: %s\n", condition ? "PASS" : "FAIL", name);

    if (condition)
    {
        ++passed;
    }
    else
    {
        ++failed;
    }
}

/* Supply scenario_load with a temporary file containing the test input. */
static int load_text(const char *text, scenario_t *s)
{
    char filename[64];
    FILE *file = NULL;

    for (int n = 0; n < 1000; ++n)
    {
        snprintf(filename, sizeof filename, ".hw06-test-%d.tmp", n);
        errno = 0;
        file = fopen(filename, "wx");

        if (file != NULL || errno != EEXIST)
        {
            break;
        }
    }

    if (file == NULL)
    {
        check(0, "create temporary scenario");
        return -1;
    }

    int written = fputs(text, file) != EOF;

    if (fclose(file) != 0)
    {
        written = 0;
    }

    int rc = -1;

    if (written)
    {
        rc = scenario_load(filename, s);
    }
    else
    {
        check(0, "write temporary scenario");
    }

    if (remove(filename) != 0)
    {
        check(0, "remove temporary scenario");
    }

    return rc;
}

/* This must run before iom361_initialize to test error code 5. */
static void test_uninitialized_sensor(void)
{
    reading_t reading = {(time_t)123, 12.5f, 45.5f};
    unsigned char saved[sizeof reading];
    memcpy(saved, &reading, sizeof reading);

    int rc = sensor_read(NULL, (time_t)999, &reading);

    check(rc == 5 && memcmp(saved, &reading, sizeof reading) == 0,
          "uninitialized sensor fails without changing the reading");
}

static void test_sensor(uint32_t *base)
{
    reading_t reading = {0};

    _iom361_set_sensor1(91.0f, 45.5f);
    int rc = sensor_read(base, (time_t)1791273602, &reading);

    check(rc == 0 && reading.temp_c == 91.0f,
          "91.0 C reads back as 91.0 C");
    check(rc == 0 && reading.rh_pct == 45.5f,
          "humidity is converted and rounded");
    check(rc == 0 && reading.t == (time_t)1791273602,
          "sensor stores the supplied timestamp");

    _iom361_set_sensor1(0.0f, 0.0f);
    rc = sensor_read(base, 1, &reading);

    check(rc == 0 && reading.temp_c == 0.0f && reading.rh_pct == 0.0f,
          "zero temperature and humidity");

    _iom361_set_sensor1(100.0f, 100.0f);
    rc = sensor_read(base, 2, &reading);

    check(rc == 0 && reading.temp_c == 100.0f && reading.rh_pct == 100.0f,
          "100 C and 100 percent humidity");

    _iom361_set_sensor1(-10.2f, 60.0f);
    rc = sensor_read(base, 3, &reading);

    check(rc == 0 && fabsf(reading.temp_c - (-10.2f)) < 0.001f,
          "negative temperature rounds correctly");

    _iom361_set_sensor1(91.0f, 40.0f);
    base[TEMP_REG / sizeof *base] |= UINT32_C(0xFFF00000);
    base[HUMID_REG / sizeof *base] |= UINT32_C(0xFFF00000);
    rc = sensor_read(base, 4, &reading);

    check(rc == 0 && reading.temp_c == 91.0f && reading.rh_pct == 40.0f,
          "sensor ignores bits above bit 19");

    unsigned char saved[sizeof reading];
    memcpy(saved, &reading, sizeof reading);
    rc = sensor_read(NULL, 5, &reading);

    check(rc == 1 && memcmp(saved, &reading, sizeof reading) == 0,
          "failed register read leaves the entire reading unchanged");
}

static void test_panel(uint32_t *base)
{
    check(panel_pressed(1, 0) == 1, "pressed detects a press");
    check(panel_released(1, 0) == 0, "released ignores a press");
    check(panel_pressed(0, 1) == 0, "pressed ignores a release");
    check(panel_released(0, 1) == 1, "released detects a release");
    check(panel_pressed(1, 1) == 0, "pressed ignores a held switch");
    check(panel_released(1, 1) == 0, "released ignores a held switch");

    check(panel_pressed(0xAu, 0x5u) == 0xAu &&
          panel_released(0xAu, 0x5u) == 0x5u,
          "simultaneous presses and releases remain separate");

    check(panel_pressed(UINT32_C(0x80000000), 0) == UINT32_C(0x80000000),
          "switch bit 31 is handled");

    int rc;
    iom361_writeReg(base, LEDS_REG, 0xA5u, &rc);
    check(rc == 0, "set up the LED pattern");

    check(panel_led(base, 1, 1) == 0 &&
          iom361_readReg(base, LEDS_REG, &rc) == 0xA7u && rc == 0,
          "panel_led turns one LED on and preserves the others");

    check(panel_led(base, 0, 0) == 0 &&
          iom361_readReg(base, LEDS_REG, &rc) == 0xA6u && rc == 0,
          "panel_led turns one LED off and preserves the others");

    check(panel_toggle(base, 2) == 0 &&
          iom361_readReg(base, LEDS_REG, &rc) == 0xA2u && rc == 0,
          "panel_toggle flips one LED and preserves the others");

    check(panel_toggle(base, 2) == 0 &&
          iom361_readReg(base, LEDS_REG, &rc) == 0xA6u && rc == 0,
          "toggling twice restores the LED pattern");

    check(panel_led(base, 31, 1) == 0 &&
          iom361_readReg(base, LEDS_REG, &rc) == UINT32_C(0x800000A6) && rc == 0,
          "LED bit 31 can be set");

    check(panel_toggle(base, 31) == 0 &&
          iom361_readReg(base, LEDS_REG, &rc) == 0xA6u && rc == 0,
          "LED bit 31 can be toggled");

    check(panel_led(base, -1, 1) == -1 && panel_led(base, 32, 1) == -1 &&
          iom361_readReg(base, LEDS_REG, &rc) == 0xA6u && rc == 0,
          "panel_led rejects invalid indices without changing LEDs");

    check(panel_toggle(base, -1) == -1 && panel_toggle(base, 32) == -1 &&
          iom361_readReg(base, LEDS_REG, &rc) == 0xA6u && rc == 0,
          "panel_toggle rejects invalid indices without changing LEDs");

    check(panel_led(NULL, 1, 1) == 1 && panel_toggle(NULL, 1) == 1 &&
          iom361_readReg(base, LEDS_REG, &rc) == 0xA6u && rc == 0,
          "panel functions propagate register errors without changing LEDs");
}

static void test_scenario(void)
{
    const char *text =
        "# scenario comment\n\n"
        "start 1791273600 # UTC\n"
        "0 21.0 40.0 0x0\n\n"
        "3 91.0 55.5 0x3 # two switches\n"
        "7 -10.2 60.0 0x0\n";

    scenario_t s;
    int rc = load_text(text, &s);

    check(rc == 0, "scenario accepts comments and blank lines");

    if (rc != 0)
    {
        return;
    }

    check(s.start == (time_t)1791273600 && s.count == 3,
          "scenario stores the start time and data-row count");

    check(s.lines[1].tick == 3 && s.lines[1].temp_c == 91.0f &&
          s.lines[1].rh_pct == 55.5f && s.lines[1].switches == 0x3u,
          "scenario stores all four data fields");

    check(scenario_at(&s, 0) == &s.lines[0],
          "lookup includes a row's tick");
    check(scenario_at(&s, 2) == &s.lines[0],
          "values hold before the next row");
    check(scenario_at(&s, 3) == &s.lines[1],
          "values change at the next row");
    check(scenario_at(&s, 6) == &s.lines[1],
          "values hold across a longer gap");

    check(scenario_at(&s, 7) == &s.lines[2] &&
          scenario_at(&s, 99) == &s.lines[2],
          "the final row remains in force");

    check(scenario_at(&s, -1) == NULL && scenario_at(NULL, 0) == NULL,
          "lookup returns NULL when no row is available");

    const struct
    {
        const char *name;
        const char *text;
        int line;
    } bad[] =
    {
        {"missing start reports the first bad line",
         "# comment\n\n0 21 40 0\n", 3},

        {"missing field reports its physical line",
         "# comment\nstart 0\n\n0 21 40\n", 4},

        {"equal ticks are rejected",
         "start 0\n0 21 40 0\n0 22 40 1\n", 3},

        {"decreasing ticks are rejected",
         "start 0\n5 21 40 0\n3 22 40 1\n", 3},

        {"negative ticks are rejected",
         "start 0\n-1 21 40 0\n", 2},

        {"extra fields are rejected",
         "start 0\n0 21 40 0 extra\n", 2},

        {"invalid hexadecimal is rejected",
         "start 0\n0 21 40 0xG\n", 2},

        {"switch-register overflow is rejected",
         "start 0\n0 21 40 100000000\n", 2},

        {"non-finite measurements are rejected",
         "start 0\n0 nan 40 0\n", 2},

        {"an empty file reports missing content",
         "", 1},

        {"a scenario needs at least one data row",
         "start 0\n", 2}
    };

    unsigned char saved[sizeof s];
    memcpy(saved, &s, sizeof s);

    for (size_t i = 0; i < sizeof bad / sizeof bad[0]; ++i)
    {
        rc = load_text(bad[i].text, &s);
        check(rc == bad[i].line, bad[i].name);
    }

    check(memcmp(saved, &s, sizeof s) == 0,
          "failed loads leave the previous scenario unchanged");

    rc = load_text("start 0\r\n0 21 40 F", &s);

    check(rc == 0 && s.lines[0].switches == 15,
          "CRLF, hexadecimal without 0x, and no final newline are accepted");

    memset(&s, 0, sizeof s);

    check(scenario_at(&s, 0) == NULL,
          "lookup on an empty scenario returns NULL");
}

static void test_capacity(void)
{
    char text[(SCENARIO_MAX_LINES + 1) * 32 + 32];
    size_t used = (size_t)snprintf(text, sizeof text, "start 0\n");

    for (int tick = 0; tick < SCENARIO_MAX_LINES; ++tick)
    {
        used += (size_t)snprintf(text + used, sizeof text - used,
                                 "%d 21 40 0\n", tick);
    }

    scenario_t s;
    int rc = load_text(text, &s);

    check(rc == 0 && s.count == SCENARIO_MAX_LINES,
          "scenario accepts exactly 1000 data rows");

    snprintf(text + used, sizeof text - used,
             "%d 21 40 0\n", SCENARIO_MAX_LINES);

    rc = load_text(text, &s);

    check(rc == SCENARIO_MAX_LINES + 2 && s.count == SCENARIO_MAX_LINES,
          "row 1001 is rejected without replacing the scenario");
}

int main(void)
{
    test_uninitialized_sensor();

    int rc;
    uint32_t *base = iom361_initialize(4, 4, &rc);

    if (base == NULL || rc != 0)
    {
        check(0, "initialize iom361");
        return EXIT_FAILURE;
    }

    iom361_set_display(false);

    test_sensor(base);
    test_panel(base);
    test_scenario();
    test_capacity();

    printf("%d unit tests passed, %d failed\n", passed, failed);
    return failed == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}