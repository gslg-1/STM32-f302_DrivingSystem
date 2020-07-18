#ifndef __ERRORLOGGER_H
#define __ERRORLOGGER_H

#include "logger.h"

typedef struct eLogData_s eLogData;




struct eLogData_s {
    uint16_t signature;
    uint8_t module;
    uint8_t function;
    uint8_t number;
    uint8_t value;
    uint16_t rest;
} ;


/* External Variables ------------------------------------------ */
/* Variables --------------------------------------------------- */

Log ErrorLogger;

/* External Functions */
extern void sendUartMsg(char * str, uint8_t length);
extern void Error_Handler(void);
/* Function Prototyps ------------------------------------------ */
uint8_t initErrorLog(uint32_t * start, uint32_t * end);
uint8_t writeError( uint8_t module , uint8_t function , uint8_t number , uint8_t value);
uint8_t printErrorData ( void );
/* Special Functions ------------------------------------------- */
void errorStringBuilder( char * str, uint8_t strLength , uint8_t module , uint8_t function , uint8_t number , uint8_t value );
void DBG_Error_Handler( uint8_t mod , uint8_t fnx , uint8_t num , uint8_t val );

/* Function Implementation ------------------------------------- */
uint8_t initErrorLog(uint32_t * start, uint32_t * end)
{
    if (start != NULL && end > start + 2)
    {
        if (logger_init(  &ErrorLogger , (uint16_t)SIGNATURE_ERROR , 2 , start , end ) != PRG_MNG_OK)
        {
            return PRG_MNG_FAILED;
        }
        return PRG_MNG_OK;
    }
    return PRG_MNG_FAILED;
}
uint8_t writeError( uint8_t module , uint8_t function , uint8_t number , uint8_t value)
{
    eLogData ed = {
        .signature = (uint16_t)SIGNATURE_ERROR,
        .module = module,
        .function = function,
        .number = number,
        .value = value,
        .rest = (uint16_t)0xABCD
    } ;
    if (logger_add( &ErrorLogger ,  (uint32_t*)&ed ) == PRG_MNG_OK)
    {
        return PRG_MNG_OK;
    }
     return PRG_MNG_FAILED;
}

uint8_t printErrorData ( void )
{
    if (logger_switch ( &ErrorLogger , LOG_MODE_READ ) == PRG_MNG_OK )
    {
        eLogData * ed  = NULL;
        uint32_t * cur = NULL;
        uint8_t msgLength = 99;
        char msg[msgLength]; 
        cur = logger_PgetFirst( &ErrorLogger );

        while (cur != NULL )
        {
            ed = (eLogData *)cur;
            errorStringBuilder( msg , msgLength , ed->module , ed->function , ed->number , ed->value );
            sendUartMsg(msg,msgLength);
            cur = logger_PgetNext( &ErrorLogger , cur);
        }
        if (logger_switch ( &ErrorLogger , LOG_MODE_WRITE_FREE ) != PRG_MNG_OK )
        {
            Error_Handler();
        }
        return PRG_MNG_OK;
    }
    return PRG_MNG_FAILED;
}

void errorStringBuilder( char * str, uint8_t strLength , uint8_t module , uint8_t function , uint8_t number , uint8_t value )
{
    /* String clear */
    memset(str,0,strLength);
    /* String Cat */
    char mod[3];
    char func[3];
    char num[3];
    char val[3];
    itoa( module , mod , 10 );
    itoa( function , func , 10 );
    itoa( number , num , 10 );
    itoa( value , val , 10 );

    strcat( str , "Error -> ( m:" );
    strcat( str , mod );
    strcat( str , " | f:" );
    strcat( str , func );
    strcat( str , " | #:" );
    strcat( str , num );
    strcat( str , " | v:" );
    strcat( str , val );
    strcat( str , " )\n" );
}
void DBG_Error_Handler( uint8_t mod , uint8_t fnx , uint8_t num , uint8_t val )
{
  writeError( mod , fnx , num , val);
  sendUartMsg( "New Error Handler\n" , sizeof("New Error Handler\n") );
  Error_Handler();
}

#endif