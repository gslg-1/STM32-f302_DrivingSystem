#include "example1.h"

// private varibales:

uint8_t getGreatest(uint8_t valueA, uint8_t valueB)
{
    if(valueA >= valueB)
    {
        return valueA;
    }
    return valueB;
}
uint8_t getLeast(uint8_t valueA, uint8_t valueB)
{
    if(valueA <= valueB)
    {
        return valueA;
    }
    return valueB;
}