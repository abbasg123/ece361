#include "bits.h"
#include <stdio.h>

void printBinary(uint32_t x, int w) // w = width
{//variable for for loop to run until the amount (width) of bits specified
    if (w < 1 || w > 32) 
    {//for invalid widths
        return;
    }
    int i = w-1;

    for(; i >= 0; i--)
    {//walk thru width amount of bits until the end
        int b = (x >> i) & 1u; //move every bit to position 0 lsb to determine if it is a 1 or 0, storing it in b
    
        printf("%d", b);
    
        if(((i % 4)==0)&&(i>0)) //position a space every 4 bits for typical binary syntax
        {//second condition stops trailing space
            printf(" ");
        }
    }
}

uint32_t getField(uint32_t wo, int p, int wi)// wo = word, p = position, wi = width
{
    if (p < 0 || p > 31 ||
        wi < 1 || wi > 32 || (p + wi) > 32
    )//ensure position not negative or more than string length, width not less than 0 or greater than string length, and field within allotted max "
    {
        return wo;
    }
    uint32_t shiftInput = wo >> (p); //store requested bits in shift input by shifting by positions = p
    uint32_t mask = UINT32_MAX >> (32 - wi); //create mask of all 1's, shifted such that result is all 0s except 1's of a width = wi
    return (shiftInput & mask); // all 0's except for width = wi, then 1's for a width = wi, which preserves the result of shiftInput
}

uint32_t setField(uint32_t wo, int p, int wi, uint32_t val) // ^, ^, ^
{
    if (p < 0 || p > 31 ||
        wi < 1 || wi > 32 || (p + wi) > 32
    )//ensure position not negative or more than string length, width not less than 0 or greater than string length, and field within allotted max "
    {
        return wo;
    }
    uint32_t lowMask = UINT32_MAX >> (32 - wi); //mask of 0's besides where width is all 1's (from rightmost, LSB)
    uint32_t mask = lowMask << p; //shift all the width 1's to the left by p, leaving all 0's besides 1's in p to p+(width-1)
    //the inversion of MASK can now hold 1's besides 0's where the defined field is
    // & with the original word will result in clearing that field into 0's and preserving all other bits
    uint32_t wordFieldCleared = wo & ~(mask);
    //variable to store replacement; lowMask & val results in cleared val besides lowest requested bits of val
    uint32_t lowVal = (val & lowMask) << p;//shifted by p position into correct field (already correct width since same lowMask used)

    //Since lowval is all zero's besides the correctly placed field, and wordFieldCleared holds all the original data of word except the 
    //correctly placed field being 0's, | (OR) preserves the bits of wordFieldCleared outside the field and inserts the lowest bits of val into
    //the specified field
    return wordFieldCleared | lowVal;
}

int32_t signExtend(uint32_t val, int w)//val = value, some integer
{
    if (w < 1 || w > 32)
    { //ensure width is nonempty but not longer than total length
        return 0;
    }
    //mask of rightmost w, width, bits being 1's, to the left of width 1's is 0's
    uint32_t mask = UINT32_MAX >> (32 - w);
    uint32_t bits = val & mask; // bits stores the & result, clearing everything in value besides the rightmost width amount (keep lowest bits)
    uint32_t signBit = (uint32_t)1 << (w-1); //move a 1 to highest bit available (constrained by w width)
    if((bits & signBit) == 0) //compare bits to signbit, result & determines if (==0); bits is positive
    {
        return (int32_t)(bits);
    }
    else
    {//for every amount of bits w, width, long; subtracting (shift 1[lsb] by w)2^w results in the correct 2s complement (+128 -> -128 = -256)
        int64_t signedVal = (int64_t)bits - ((int64_t)1 << w);
        return (int32_t)signedVal;
    }
}
