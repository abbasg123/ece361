#include "ptr.h"
#include <stddef.h>

void swapFloat(float *x, float *y)
{
    float t = *x; *x = *y; *y = t;
}

int minMax(const float *a, int n, float *min, float *max)
{
    //ensure array has at least 1 value (n>=1), and no pointers == nullptr
    if((n < 1) || a == NULL || min == NULL|| max == NULL)
    {
        return -1;
    }

    const float *p = a;
    float lmin = *p;
    float lmax = *p;
    for(p = a+1; p < (n + a); p++)
    {
        if(*p < lmin)
        {
            lmin = *p;
        }
        if(*p > lmax)
        {
            lmax = *p;
        }
    }
    *min = lmin;
    *max = lmax;
    return 0;
}

const float *findAbove(const float *a, int n, float thresh)
{//walk through a to find & return the address of the first member of a who is larger than thresh
    if(n < 1 || a == NULL)
    {
        return NULL;
    }
    const float *p;
    for(p = a; p < (a+n); p++)
    {
        if(*p > thresh)
        {
            return p;
        }
    }
    return NULL;
}
void reverse(float *a, int n)
{//save two pointers at the first and last entry of a, swap until the pointers are next to each other
    if(a == NULL || n < 1)
    {
        return;
    }
    float *first = a;
    float *last = (a + n) - 1;
    while(first < last)
    {
        swapFloat(first, last);
        first++;
        last--;
    }
}
