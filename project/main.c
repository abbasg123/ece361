#include <stdint.h>
#include <stdio.h>

#include "sensor.h"
#include "iom361_r4.h"

int main(void)
{
    int rc;
    uint32_t *base = iom361_initialize(2, 3, &rc);
    if (base == NULL || rc != 0)
    {
        fprintf(stderr, "iom361_initialize failed: %d\n", rc);
        return 1;
    }

    /* Simulate ordinary conditions at the store entrance. */
    _iom361_set_sensor1(21.0f, 45.0f);
    reading_t reading = { .timeStamp = (time_t)1791273600 };
    rc = sensor_read(base, &reading.tempC, &reading.rhPCT);
    if (rc != 0)
    {
        fprintf(stderr, "sensor_read failed: %d\n", rc);
        return 1;
    }

    printf("Automatic door: temperature=%.1f C humidity=%.1f %%RH\n",
           reading.tempC, reading.rhPCT);

    return 0;
}
