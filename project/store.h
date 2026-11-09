#ifndef STORE_H
#define STORE_H

#include "sensor.h"
#include <stddef.h>
#include <stdint.h>
/*
 * Store each door reading, state, and uint32_t status word as a record_t.
 * Hide storage behind an opaque store_t interface; eventually use a BST.
 * Order records by temperature in 0.5-degree C bins (K2), preserving duplicates.
 * Support creation, insertion, key lookup, in-order traversal, count,
 * height, comparison through record_compare, and destruction.
 */

//states, start at 0
typedef enum
{
    CLOSED,
    OPENING,
    HELD_OPEN,
    CLOSING,
    SAFETY_STOP
} state_t;

typedef struct
{
    reading_t reading;
    state_t state;
    uint32_t status;
} record_t;

//only store.c can change, opaque
typedef struct store store_t;

/*
record points to current entry
context passes caller data (&counter)

*/
typedef void (*store_visit_fn)(const record_t *record, void *context);

/* order records by 0.5C intvl.
 * Records must be non-NULL with non-Infinite temperatures.
 * Returns -1, 0, or 1; 
 * timestamps, humidity, state, status no affect. */
int record_compare(const record_t *left, const record_t *right);

/* Allocates an empty history.
   Return addr, or null if no mem
   Must pass address to store_destroy(); 
*/
store_t *store_create(void);


/* save copy of *record , growing when full.
   Retain duplicates, earlier insertions before later.

   store/record non null/infinite. returns 0 for success,
   -1 for bad input or no mem.
   Failure preseves history, success allows reuse
*/
int store_insert(store_t *store, const record_t *record);

/* Find the first record in the interval containing temp_c, or NULL.
 * Returned pointers remain valid until the next insert or destroy. */
const record_t *store_find(const store_t *store, float temp_c);

/* Visit every record with this key; returns the number visited.
 * A NULL callback counts matches without calling anything. */
size_t store_find_all(const store_t *store, float temp_c,
                      store_visit_fn visit, void *context);

/* Visit in key order. Callbacks must not insert into or destroy the store. */
void store_walk(const store_t *store, store_visit_fn visit, void *context);
size_t store_count(const store_t *store);

/* Temporary array has no tree height: returns 0. The week-8 tree will
 * return its actual height (empty 0, one node 1). */
size_t store_height(const store_t *store);
void store_destroy(store_t *store);




#endif
