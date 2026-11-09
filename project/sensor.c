#include "sensor.h"
#include "iom361_r4.h"
#include <math.h>
#include <stddef.h>

int sensor_read(uint32_t *base, float *tempc, float *rhpct)
{
    if(rhpct == tempc || tempc == NULL || rhpct == NULL)
    {
        return -1;
    }
    //each register sets rc, returns 0xDEADBEEF on error
    int rc;
    uint32_t rTemp = iom361_readReg(base, TEMP_REG, &rc);
    if(rc != 0)
        return rc;
    uint32_t rHumid = iom361_readReg(base, HUMID_REG, &rc);
    if(rc != 0)
        return rc;
    
    /* The AHT20 stores fractions of 2^20 = 1048576. Its temperature scale
     * spans 200 C starting at -50 C; 
       its humidity scale spans 100 %RH.
     * 
       Wait until both reads succeed before writing either caller variable. */
    float temp = rTemp / 1048576.0f * 200.0f - 50.0f;
    float humid = rHumid / 1048576.0f * 100.0f;
    *tempc = roundf(temp * 10.0f) / 10.0f;
    *rhpct = roundf(humid * 10.0f) / 10.0f;
    return 0;


}