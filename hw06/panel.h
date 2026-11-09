#ifndef PANEL_H
#define PANEL_H

#include <stdint.h>

/* Return one bit per switch that changed from 0 to 1. */
uint32_t panel_pressed(uint32_t now, uint32_t before);

/* Return one bit per switch that changed from 1 to 0. */
uint32_t panel_released(uint32_t now, uint32_t before);

/*
 * Set LED n; nonzero on means lit. Preserve every other LED.
 * Return 0, -1 for n outside 0..31, or the failed iom361 operation's code.
 */
int panel_led(uint32_t *base, int n, int on);

/* Toggle LED n, preserving the others; return codes as for panel_led. */
int panel_toggle(uint32_t *base, int n);

#endif
