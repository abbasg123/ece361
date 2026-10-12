#ifndef STORE_H
#define STORE_H

/*
 * Store each door reading, state, and uint32_t status word as a record_t.
 * Hide storage behind an opaque store_t interface; eventually use a BST.
 * Order records by temperature in 0.5-degree C bins (K2), preserving duplicates.
 * Support creation, insertion, key lookup, in-order traversal, count,
 * height, comparison through record_compare, and destruction.
 */

#endif
