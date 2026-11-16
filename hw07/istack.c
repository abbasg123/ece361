#include "istack.h"

#include <stdint.h>
#include <stdlib.h>

struct istack
{
    int *data;        /* Owned array: active values occupy indices 0..count-1. */
    size_t count;     /* Also the index where the next push stores its value. */
    size_t capacity;  /* Allocated integer slots; never decreases. */
};

istack_t *istack_create(void)
{
    //create space for structure
    istack_t *s = malloc(sizeof *s);
    if (s == NULL) //if allocation fails
    {
        return NULL; //return error
    }

    //create space for array  /** 8 * size of 1 int  */
    s->data = malloc(ISTACK_START * sizeof *s->data);
    if (s->data == NULL)
    {
        /* The structure was allocated, but the stack cannot be completed. */
        free(s); //so release allocated
        return NULL; //return error
    }

    //set count (of elements) to 0, capacity to 8, and return address of structure 
    s->count = 0;
    s->capacity = ISTACK_START;
    return s;
}

void istack_destroy(istack_t *s)
{//creation obtains two allocations so we free both; array in data and s, structure
    if (s == NULL)
    {
        return;
    }
    //since data array in structure, data array released first 
    /* Read the array pointer and release it before releasing its structure. */
    free(s->data);
    free(s);
}

int istack_push(istack_t *s, int v)
{
    if (s == NULL)
    {//ensure stack exists 
        return -1;
    }

    if (s->count == s->capacity)
    {
        /* Ensure doubling the slots and converting to bytes cannot wrap. */
        size_t max_capacity = SIZE_MAX / sizeof *s->data;
        if (s->capacity > max_capacity / 2)
        {
            return -1;
        }

        size_t new_capacity = s->capacity * 2;
        int *grown_data = realloc(s->data, new_capacity * sizeof *s->data);
        if (grown_data == NULL)
        {
            /* Failed realloc leaves the original allocation usable. */
            return -1;
        }

        /* Commit the new address and capacity only after growth succeeds. */
        s->data = grown_data;
        s->capacity = new_capacity;
    }

    s->data[s->count] = v;
    s->count++;
    return 0;
}

int istack_pop(istack_t *s, int *v)
{
    if (s == NULL || s->count == 0)
    {
        return -1;
    }

    /* The old top becomes the first unused slot; no erasure is needed. */
    s->count--;
    if (v != NULL)
    {
        *v = s->data[s->count];
    }

    return 0;
}

int istack_peek(const istack_t *s, int *v)
{
    if (s == NULL || s->count == 0)
    {
        return -1;
    }

    if (v != NULL)
    {
        *v = s->data[s->count - 1];
    }

    return 0;
}

size_t istack_count(const istack_t *s)
{
    if (s == NULL)
    {
        return 0;
    }

    return s->count;
}

bool istack_is_empty(const istack_t *s)
{
    return istack_count(s) == 0;
}

size_t istack_capacity(const istack_t *s)
{
    if (s == NULL)
    {
        return 0;
    }

    return s->capacity;
}

int *istack_to_array(const istack_t *s, size_t *n)
{
    if (n != NULL)
    {
        *n = 0;
    }

    if (s == NULL || s->count == 0)
    {
        return NULL;
    }

    int *copy = malloc(s->count * sizeof *copy);
    if (copy == NULL)
    {
        return NULL;
    }

    for (size_t i = 0; i < s->count; i++)
    {
        copy[i] = s->data[i];
    }

    if (n != NULL)
    {
        *n = s->count;
    }

    return copy;
}
