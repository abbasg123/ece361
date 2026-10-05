#include "status.h"
#include "bits.h"

enum 
{
    HEAT_POS = 0,
    HEAT_WIDTH = 1,

    COOL_POS = 1,
    COOL_WIDTH = 1,
    
    FAN_POS = 2,
    FAN_WIDTH = 1,
    
    FAULT_POS = 3,
    FAULT_WIDTH = 1,
    
    MODE_POS = 4,
    MODE_WIDTH = 3,
    
    RES_POS = 7,
    RES_WIDTH = 1,
    
    SETPNT_POS = 8,
    SETPNT_WIDTH = 8
};

status_t status_unpack(uint16_t w)
{
    status_t s;
    // single bit params convert to false for 0 / true for 1
    s.heat = getField(w, HEAT_POS, HEAT_WIDTH);
    s.cool = getField(w, COOL_POS, COOL_WIDTH);
    s.fan = getField(w, FAN_POS, FAN_WIDTH);
    s.fault = getField(w, FAULT_POS, FAULT_WIDTH);
    s.mode = getField(w, MODE_POS, MODE_WIDTH);
    s.res = getField(w, RES_POS, RES_WIDTH);

    //find setpoint, interpret the 8 bits as twos complement
    s.setpnt = signExtend(getField(w, SETPNT_POS, SETPNT_WIDTH), SETPNT_WIDTH);
    s.modeValidity = s.mode <= MODE_FANO;
    return s;
}