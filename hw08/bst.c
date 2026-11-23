#include "bst.h"

#include <stdlib.h>

struct node
{
    int data;

    struct node *leftC;
    struct node *rightC;
};

struct bst
{
    struct node *root;
    size_t count;
};

bst_t *bst_create(void)
{
    bst_t *t = malloc(sizeof *t);
    if(t == NULL)
    {//malloc failure
        return NULL;
    }

    t->root = NULL;
    t->count = 0;
    return t;
}

static void destroy_nodes(struct node *current)
{//private recursive helper: free a child before the node holding its address
    if (current == NULL) {
        return;
    }

    destroy_nodes(current->leftC);
    destroy_nodes(current->rightC);
    free(current);
}

void bst_destroy(bst_t *t)
{
    if (t == NULL) 
    {
        return;
    }

    destroy_nodes(t->root);
    free(t);
}

int bst_insert(bst_t *t, int key)
{
    if (t == NULL) 
    {//confirm tree exists
        return -1;
    }

    //allocate for one node
    struct node *temp = malloc(sizeof *temp);
    if (temp == NULL) 
    {//incase malloc failure
        return -1;
    }

    //initial values for node:
    temp->data = key;
    temp->leftC = NULL;
    temp->rightC = NULL;

    if (t->root == NULL) 
    {
        t->root = temp;
    } else //if there is a root: 
    {
        struct node *current = t->root;//start current at root
        struct node *parent;//reserve parent

        while (1) 
        {
            parent = current;

            if (key < parent->data) 
            {//left
                current = current->leftC;
                if (current == NULL) 
                {//if left child doesnt exist; insert it
                    parent->leftC = temp;
                    break;
                }
            } else 
            {//right, incl. equal (duplicate)
                current = current->rightC;
                if (current == NULL) 
                {//if right child doesnt exist; insert it
                    parent->rightC = temp;
                    break;
                }
            }
        }
    }

    t->count++;
    return 0;
}


bool bst_contains(const bst_t *t, int key, size_t *visits)
{
    if (visits != NULL)
    {
        *visits = 0;
    }

    if (t == NULL || t->root == NULL)
    {
        return false;
    }

    const struct node *current = t->root;

    while (current->data != key)
    {
        if (visits != NULL)
        {
            (*visits)++;
        }

        if (current->data > key)
        {
            current = current->leftC;
        }
        else
        {
            current = current->rightC;
        }

        if (current == NULL)
        {
            return false;
        }
    }

    /* The matching node was examined but skipped the loop body. */
    if (visits != NULL)
    {
        (*visits)++;
    }

    return true;
}

size_t bst_size(const bst_t *t)
{
    if (t == NULL)
    {
        return 0;
    }

    return t->count;
}
static size_t node_height(const struct node *current)
{
    if (current == NULL)
    {
        return 0;
    }

    size_t leftHeight = node_height(current->leftC);
    size_t rightHeight = node_height(current->rightC);

    if (leftHeight > rightHeight)
    {
        return 1 + leftHeight;
    }

    return 1 + rightHeight;
}


size_t bst_height(const bst_t *t)
{
    if (t == NULL)
    {
        return 0;
    }

    return node_height(t->root);
}