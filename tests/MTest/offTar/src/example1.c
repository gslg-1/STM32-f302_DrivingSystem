#include "example1.h"


uint8_t getGreatest(uint8_t valueA, uint8_t valueB)
{
    if(valueA >= valueB)
    {
        return valueA;
    }
    return valueB;
}