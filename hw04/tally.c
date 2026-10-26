#include "tally.h"
#include <stdbool.h>
#include <string.h>


typedef struct
{

    char name[TALLY_LEN + 1]; //+1 incl. null term. char
    int count;

} tallyEntry_t;

struct tally
{
    tallyEntry_t entries[TALLY_NAMES];
    int size;
    bool inUse;
};

//private pool lasting entire program
static tally_t tallyPool[TALLY_MAX];

tally_t *tally_create(void)
{
    //visit each pool member
    for (int i = 0; i < TALLY_MAX; i++)
    {
        //if free
        if (!tallyPool[i].inUse)
        {
            // clear names 
            tallyPool[i].size = 0;

            //reserve
            tallyPool[i].inUse = true;

            //return Address of member
            return &tallyPool[i];
        }
    }

    //all members reserved
    return NULL;
}

void tally_destroy(tally_t *t)
{
    //tally pointer valid / empty
    if (t == NULL)
    {
        return;
    }

    //clear entry
    t->size = 0;

    //clear reserved
    t->inUse = false;
}
int tally_add(tally_t *t, const char *name)
{
    //check pointers empty
    if (t == NULL || name == NULL)
    {
        return -1;
    }
    //
    //measure name for conditional, ensure not 0 or too long
    if (strlen(name) == 0 || strlen(name) > TALLY_LEN)
    {
        return -1;
    }

    //find correct position
    int position = 0;

    while (position < t->size && strcmp(t->entries[position].name, name) < 0)
    {
        position++;
    }

    //match name and increase count
    if (position < t->size && strcmp(t->entries[position].name, name) == 0)
    {
        t->entries[position].count++;
        return t->entries[position].count;
    }

    //check if space for new entry
    if (t->size == TALLY_NAMES)
    {
        return -1;
    }

    //"backfill" existing entries to make space
    for (int i = t->size; i > position; i--)
    {
        t->entries[i] = t->entries[i - 1];
    }

    //ensure space for null term char
    memcpy(t->entries[position].name, name, strlen(name) + 1);

    t->entries[position].count = 1;
    t->size++;

    return t->entries[position].count;
}

int tally_count(const tally_t *t, const char *name)
{
    //0 for null entries
    if (t == NULL || name == NULL)
    {
        return 0;
    }

    for (int i = 0; i < t->size; i++)
    {
        int c = strcmp(t->entries[i].name, name);

        if (c == 0)
        {
            return t->entries[i].count;
        }

      //name wouldve been found so it doesnt exist
        if (c > 0)
        {
            return 0;
        }
    }

    return 0;
}

int tally_size(const tally_t *t)
{
    if (t == NULL)
    {
        return 0;
    }

    return t->size;
}

void tally_print(const tally_t *t, FILE *out)
{
    if (t == NULL || out == NULL)
    {
        return;
    }

  //automatically sorted from strcmp
    for (int i = 0; i < t->size; i++)
    {
        fprintf(out, "%s %d\n", t->entries[i].name, t->entries[i].count);
    }
}
