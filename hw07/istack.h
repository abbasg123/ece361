#ifndef ISTACK_H
#define ISTACK_H
#include <stddef.h>
#include <stdbool.h>

#define ISTACK_START 8 /* room at first; doubles when full */

typedef struct istack istack_t; /* opaque */

istack_t *istack_create(void);

void istack_destroy(istack_t *s);

int istack_push(istack_t *s, int v);

int istack_pop(istack_t *s, int *v);

int istack_peek(const istack_t *s, int *v);

size_t istack_count(const istack_t *s);

bool istack_is_empty(const istack_t *s);

size_t istack_capacity(const istack_t *s);

int *istack_to_array(const istack_t *s, size_t *n);

#endif 