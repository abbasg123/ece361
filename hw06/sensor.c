#include "sensor.h"
#include "iom361_r4.h"
#include <math.h>

int sensor_read(uint32_t *base, time_t now, reading_t *r)
{
    int rc;
    uint32_t raw_temp = iom361_readReg(base, TEMP_REG, &rc);
    if (rc != 0)
    {
        return rc;
    }

    uint32_t raw_rh = iom361_readReg(base, HUMID_REG, &rc);
    if (rc != 0)
    {
        return rc;
    }

    /* AHT20 measurements occupy only the low 20 bits. */
    raw_temp &= UINT32_C(0x000FFFFF);
    raw_rh &= UINT32_C(0x000FFFFF);

    double temp = raw_temp / 1048576.0 * 200.0 - 50.0;
    double rh = raw_rh / 1048576.0 * 100.0;

    reading_t sample;
    sample.t = now;
    sample.temp_c = (float)(round(temp * 10.0) / 10.0);
    sample.rh_pct = (float)(round(rh * 10.0) / 10.0);

    /* Publish only a complete sample; failed reads leave *r unchanged. */
    *r = sample;
    return 0;
}
