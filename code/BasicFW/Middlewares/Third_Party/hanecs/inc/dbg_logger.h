#ifndef __DBG_LOGGER_H
#define __DBG_LOGGER_H

#include "dbg_src.h"

/* External Variables ---------------------------------------------- */
/* External Functions ---------------------------------------------- */
extern uint8_t flash_erase32( uint32_t * block_p );                         /* erase block with 32 bits */
extern uint8_t flash_write32( uint32_t * block_p, uint32_t data);          /* write block with 32 bits */
/* Public Typedefinitions ------------------------------------------ */
typedef struct Log_s Log;
typedef struct LogData_s LogData;
/* Public Structs -------------------------------------------------- */
enum Logger_Mode_s {
    LOG_MODE_WRITE_BLOCKED,
    LOG_MODE_WRITE_FREE,
    LOG_MODE_READ
} ;
struct Log_s
{
    uint8_t mode;
    uint16_t signature;
    uint8_t logSize32;
    uint32_t * p_start;
    uint32_t * p_end;
} ;


/* Public Function Prototyps --------------------------------------- */
uint8_t logger_init( Log * log , uint16_t signature , uint8_t logSize32 , uint32_t * p_start , uint32_t * p_end );
/* static length --------------------------------------------------- */
uint8_t logger_add( Log * log ,  uint32_t * DataBuffer );
uint8_t logger_switch ( Log * log , uint8_t mode );
uint32_t * logger_PgetFirst( Log * log );
uint32_t * logger_PgetNext( Log * log , uint32_t * p );

#endif