#ifndef TALLY_H
#define TALLY_H

#include <stdio.h>

#define TALLY_MAX 4 /* tallies that can exist at the same time */
#define TALLY_NAMES 16 /* different names one tally can hold */
#define TALLY_LEN 15 /* longest name, without the '\0' */

typedef struct tally tally_t; 

tally_t *tally_create(void);

void tally_destroy(tally_t *t);

int tally_add(tally_t *t, const char *name);

int tally_count(const tally_t *t, const char *name);

int tally_size(const tally_t *t);

void tally_print(const tally_t *t, FILE *out);

#endif