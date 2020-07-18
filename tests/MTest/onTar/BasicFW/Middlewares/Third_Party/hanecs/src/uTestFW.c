#include "uTestFW.h"


void assertionEqual_uint8(char * testName, uint8_t size, uint8_t exp, uint8_t fxnResult)
{
  char sValue[4] = {0};
  sendUartMsg(testName ,size);
  if(exp == fxnResult)
  {
    sendUartMsg(" - Sucess : exp = " ,sizeof(" - Sucess : exp = "));
  }
  else
  {
    sendUartMsg(" - Failed : exp = " ,sizeof(" - Failed : exp = "));
  } 
  itoa(exp,sValue,10);
  sendUartMsg(sValue , 4);
  sendUartMsg("; fxnRes = " ,sizeof("; fxnRes = "));
  memset(sValue,0,4);
  itoa(fxnResult,sValue,10);
  sendUartMsg(sValue , 4);
  sendUartMsg("\n" ,sizeof("\n"));
}