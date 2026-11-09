#ifndef SENSOR_H
#define SENSOR_H
#include <stdint.h>
#include <time.h>

 typedef struct
{
    /* data */
    time_t timeStamp;
    float tempC;
    float rhPCT;

} reading_t;
 
/* Convert the hdwr raw numbers into measurements the door can use.
 * base: address returned by iom361_initialize. 
   tempc/rhPCT: addresses of separate writable floats receiving C and %RH, rounded to one decimal place.
 * Returns 0 on success or the iom361 error code; bad output pointers return 1.
 * On any error, both caller variables keep their previous values. */

int sensor_read(uint32_t *base, float *tempc, float *rhpct);

#endif
