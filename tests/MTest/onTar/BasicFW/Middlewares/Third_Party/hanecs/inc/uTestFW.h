#ifndef __uTestFW_H
#define __uTestFW_H


#include "stdlib.h"
#include "stdint.h"
#include "string.h"


extern void sendUartMsg(char * str, uint8_t length);

void assertionEqual_uint8(char * testName, uint8_t size, uint8_t exp, uint8_t fxnResult);

#endif