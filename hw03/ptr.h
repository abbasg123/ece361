#ifndef PTR_H
#define PTR_H
//accepts two pointers to two floats, uses one temp var to swap the floats that x and y point to
void swapFloat(float *x, float *y); 
//accepts an address to an array of floats, the n number of entries, and the addresses where the minimum and maximum will be stored
//assumes a doesnt point to null and has atleast 1 entry, the addresses must not be null
int minMax(const float *a, int n, float *min, float *max); 
//accepts an address  to an array of floats, the number of entries, and the threshold
//loops through the array to find the first entry greater than thresh
//assumes a doesnt point to null and has atleast 1 entry
const float *findAbove(const float *a, int n, float thresh);
//accepts an address to an array of floats and the n number of entries
//uses swapFloat to reverse the entry
void reverse(float *a, int n);

#endif