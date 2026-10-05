#ifndef BITS_H
#define BITS_H

#include <stdint.h>

void printBinary(uint32_t x, int w); // w = width
uint32_t getField(uint32_t wo, int p, int wi);// wo = word, p = position, wi = width
uint32_t setField(uint32_t wo, int p, int wi, uint32_t val); // ^, ^, ^
int32_t signExtend(uint32_t val, int w);

#endif