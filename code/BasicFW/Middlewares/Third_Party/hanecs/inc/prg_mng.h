#ifndef __PRG_MNG_H
#define __PRG_MNG_H
/* includes */
#include "dbg_src.h"

/* typedefs */

typedef struct state_s state;
typedef struct prg_handle_s prg_handle;

typedef void (*action)(void);
typedef const state * (*transition)(void);


/* External Functions */
/* Enums */


/* Publice Types */
struct state_s {
    const action act;
    const transition tran_t;
} ;

struct prg_handle_s {
    const state * current;
} ;



/* Public Function Prototyps */
uint8_t prgMng_init( prg_handle * hprg , const state * init);
uint8_t prgMng_deinit( prg_handle * hprg);

const state * prgMng_getState( prg_handle * hprg );
uint8_t prgMng_checkTrans( prg_handle * hprg );


#endif