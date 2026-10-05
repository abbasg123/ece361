#ifndef STATUS_H
#define STATUS_H

#include <stdbool.h>
#include <stdint.h>

enum 
{
    MODE_OFF = 0,
    MODE_HEAT = 1,
    MODE_COOL = 2,
    MODE_AUTO = 3,
    MODE_FANO = 4,
};

typedef struct
{
    bool heat, cool, fan, fault, res, modeValidity;
    uint32_t mode;
    int32_t setpnt;
} status_t;

status_t status_unpack(uint16_t w);


#endif