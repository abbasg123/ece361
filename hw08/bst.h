#ifndef BST_H
#define BST_H

#include <stdbool.h>
#include <stddef.h>

typedef struct bst bst_t;

//return new empty tree (Or NULL on malloc failure)
bst_t *bst_create(void);

//free all nodes and the tree itself
void bst_destroy(bst_t *t);

//insert val, duplicates kept by taking right branch on equality
int bst_insert(bst_t *t, int key);

//search for val present, store # of nodes compared
bool bst_contains(const bst_t *t, int key, size_t *visits);

//# of stored keys incl. duplicates
size_t bst_size(const bst_t *t);

//# of nodes on longest root to parent path
size_t bst_height(const bst_t *t);

#endif 