#include "dbg_logger.h"

/* Private Function Prototyps ------------------------------------------------ */
uint8_t writeLog( Log * log , uint32_t * pos , uint32_t * dataBuffer );
uint8_t eraseLog( Log * log , uint32_t * pos );

/* Private Function Implementations ------------------------------------------ */
uint8_t writeLog( Log * log , uint32_t * pos , uint32_t * dataBuffer )
{
    uint8_t logSize = log->logSize32;
    if ( pos >= log->p_start && pos + logSize <= log->p_end )
    {
        for ( uint8_t i = 0  ; i < logSize ; i++ )
        {
            if (flash_write32( pos + i , *(dataBuffer + i) ) == PRG_MNG_FAILED )      
            {
                return PRG_MNG_FAILED;
            }
        }
        return PRG_MNG_OK;
    }
    return PRG_MNG_FAILED;
}
uint8_t eraseLog( Log * log , uint32_t * pos )
{
    uint8_t logSize = log->logSize32;
    if ( pos >= log->p_start && pos + logSize <= log->p_end )
    {
        for ( uint8_t i = 0  ; i < logSize ; i++ )
        {
            flash_erase32( pos + i );
        }
        return PRG_MNG_OK;
    }
    return PRG_MNG_FAILED;
}



/* Public Function Implementations ------------------------------------------- */
uint8_t logger_init( Log * log , uint16_t signature , uint8_t logSize32 , uint32_t * p_start , uint32_t * p_end )
{
    if (log != NULL && signature > (uint16_t)0 && logSize32 > (uint8_t)0 && p_start > (uint32_t*)0 && p_end > p_start + logSize32)
    {
        log->signature = signature;
        log->logSize32 = logSize32;
        log->p_start = p_start;
        log->p_end = p_end;
        log->mode = LOG_MODE_WRITE_FREE;
        return PRG_MNG_OK;
    }
    return PRG_MNG_FAILED;
}

/* static length --------------------------------------------------- */
uint8_t logger_add( Log * log ,  uint32_t * dataBuffer )
{
    if (log != NULL && log->mode == LOG_MODE_WRITE_FREE && log->signature != 0)
    {
        uint32_t * start = log->p_start;
        uint32_t * end = log->p_end;
        
        uint32_t * p_cur = start;
        uint8_t logSize = log->logSize32;
        
        log->mode = LOG_MODE_WRITE_BLOCKED;
        
        if (p_cur == NULL)
        {
            return PRG_MNG_FAILED;
        }

        /* find next free location */
        while( *((uint16_t*)p_cur) == log->signature && p_cur <= end )
        {
            p_cur += logSize;
        }
        /* Check if enought space left */
        if (p_cur + logSize > end)
        {
            p_cur = start;
        }
        /* add item */
        if (writeLog( log , p_cur , dataBuffer) == PRG_MNG_FAILED)
        {
            return PRG_MNG_FAILED;
        }
        /* Remove space after the item*/
        if ( p_cur + logSize > end)
        {
            p_cur = start;
        }
        else
        {
            p_cur += logSize;
        }

        if (eraseLog( log , p_cur) == PRG_MNG_FAILED)
        {
            return PRG_MNG_FAILED;
        }
        log->mode = LOG_MODE_WRITE_FREE;
        return PRG_MNG_OK;
    }
    return PRG_MNG_FAILED;
}
uint8_t logger_switch ( Log * log , uint8_t mode )
{
    if (log != NULL && log->mode != LOG_MODE_WRITE_BLOCKED )
    {
        log->mode = mode;
        return PRG_MNG_OK;
    }
    return PRG_MNG_FAILED;
}
uint32_t * logger_PgetFirst( Log * log ) 
{
    if ( log != NULL && log->mode == LOG_MODE_READ )
    {
        /* Find the free space */
        uint32_t * start = log->p_start;
        uint32_t * end = log->p_end;
        uint32_t * cur = start;
        /* Find the free space */
        while ( *((uint16_t *)cur) == log->signature && cur <= end )
        {
            cur++; 
        }
        /* Find the first element after the free space */
        while ( *((uint16_t *)cur) != log->signature && cur <= end )
        {
            cur++;
        }
        /* Check if there was a element after the free space*/
        if ( *((uint16_t *)cur) == log->signature && cur <= log->p_start)
        {
            sendUartMsg("DBG 1\n",sizeof("DBG 1\n"));
            return cur;
        }
        else if ( *(uint16_t*)start == log->signature  )
        {
            return cur;
        }
        /* else the whole mem is empty*/
    }
    return NULL;
}
uint32_t * logger_PgetNext( Log * log , uint32_t * p )
{
    if ( log != NULL && log->mode == LOG_MODE_READ && p != NULL )
    {
        if( p + log->logSize32 <= log->p_end)
        {
            p += log->logSize32;
        }else
        {
            p = log->p_start;
        }
        if ( *((uint16_t *)p) == log->signature )
        {
            return p;
        }
    }
    
    return NULL;
}
