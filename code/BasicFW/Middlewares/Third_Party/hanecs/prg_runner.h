#ifndef __PRG_RUNNER_H
#define __PRG_RUNNER_H
/**
*/

#include "prg_mng.h"

/* Definitions */

/* Typedefs */
typedef enum prgs_e prgs;
/* Structures */
enum prgs_e
{
    PRG_Demo = 0,               // Program Demo
    PRG_CErr,                   // Program Create Errors
    PRG_HEra,                   // Program Erase HNCS Flash Block
    PRG_TNext,                  // Program Erase HNCS Flash Block
    PRG_PErr,                   // Program Print All Errors
    _PRG_END
};

/*Extern Function Toolset */
extern void DBG_Error_Handler( uint8_t mod , uint8_t fnx , uint8_t num , uint8_t val );
extern void sendUartMsg(char * str, uint8_t length);
extern void setTimer(uint32_t value);

extern uint8_t getButtonState(void);
extern uint8_t getPrgTimer0Value(void);

extern uint8_t writeError( uint8_t module , uint8_t function , uint8_t number , uint8_t value);
extern uint8_t printErrorData ( void );
extern uint8_t flash_eraseHNCS( void );
extern uint8_t flash_writeNext( void );

extern void resetUartCom(void);

/*Intern Function Toolset*/
/* Setter */
uint8_t setCurPrg(uint8_t prg);
/* Getter */
uint8_t getCurPrg(void);

const state * getStateOfPrg(uint8_t prg);

/* Variables */
prgs curPrg = PRG_Demo;
uint32_t prgMng_timer0;

/* Dummy */
void actDoNothing(void){};
uint8_t immediately(void){ return 1; };

/* External Variables */

/* Actions */
/* Program Selection - Button */
void actPrintCurrentPrg(void);
void actNextPrg(void);
void actSetTimer(void);
/* Program Selection - UART */
void actSendAck(void);
void actSendNAck(void);

/* Program Start */
void actPrgStartMsg(void);

/* Program Demo */
void actDemoEnter(void);
/* Program Erase HNCS Flash Block */
void actHEraEnter(void);
/* Test Program - Write Test Package to Flash */
void actTFNextEnter(void);
/* Program Print Error */
void actPErrEnter(void);
/* Program Create Error */
void actCErrEnter(void);
void actCErrBase(void);
void actCErrOr(void);
void actCErrCreate(void);

/* Conditions*/
extern uint8_t b1Pre(void);
extern uint8_t timerEq0(void);
extern uint8_t timerGt0(void);

extern uint8_t uartCmdIsAvailable(void);
extern uint8_t getUartCmd(void);


uint8_t b1Rel(void);
uint8_t prgEqDemo(void);
uint8_t prgEqTNext(void);
uint8_t prgEqHEra(void);
uint8_t prgEqCErr(void);
uint8_t prgEqPErr(void);
uint8_t b1PreAndTimerEq0(void);
uint8_t b1RelAndTimerGt0(void);
uint8_t b1RelAndTimerEq0(void);


const state * check_sPrgShow(void);
const state * check_sPrgSwtch(void);
const state * check_sPrgSwSh(void);
const state * check_sUartCmd(void);
const state * check_sSendACK(void);
const state * check_sSendNACK(void);
const state * check_sPrgStart(void);
const state * check_bsPrgShow(void);
const state * check_sPrgCErr(void);
const state * check_sPrgCErrB(void);
const state * check_sPrgCErrO(void);
const state * check_sPrgCErrC(void);


prg_handle hPrg1;
/* ------------------ Program Selection - Button ------------------ */
/** 
 * State sPrgShow
 * @state: Show Current Program
 * @action: Shows the current Program via UART
 * @transition:
 * ----(b)-------------------------------------------> (sPrgShSw)
 *   |
 *   --(uart cmd available & fit to prg)-------------> (sUartCmd)
 * 
*/
const state sPrgShow =
{
    .act = &actPrintCurrentPrg,
    .tran_t = &check_sPrgShow,
} ;

/** 
 * State sPrgSwtch
 * @state: Switch Programm
 * @action: 
 *  - Switch to the next Prg
 * @transition:
 * ----(!b)-----------------------------------------> (sPrgShow)
 * 
*/
const state sPrgSwtch =
{
    .act = &actNextPrg,
    .tran_t = &check_sPrgSwtch,
} ;

/** 
 * State sPrgSwSh 
 * @state: Fork to Swich or Show Prg
 * @action: 
 *  - Starts a timer
 * @transition:
 * ----(!b)----------------------------------------> (sPrgSwtch)
 *   |
 *   --(timer = 0 )--------------------------------> (sPrgStart)
 * 
*/

const state sPrgSwSh =
{
    .act = &actSetTimer,
    .tran_t = &check_sPrgSwSh,
} ;

/* ------------------ Program Selection - UART ------------------ */
/** 
 * State sUartCmd
 * @state: Release Receive for new Uart Msg
 * @action: 
 *  - do nothing
 * @transition:
 * ----(cmd known)---------------------------------------------> (sSendACK)
 *  |
 *  ---(cmd unknown)-------------------------------------------> (sSendNACK)
*/
const state sUartCmd =
{
    .act = &actDoNothing,
    .tran_t = &check_sUartCmd,
} ;

/** 
 * State sSendACK
 * @state: Send Acknowledge and load Prg
 * @action: 
 *  - Send Acknowledge
 *  - Load Prg
 *  - Free Receiver
 * @transition:
 * ----(cmd equal prg 1)---------------------------------------> (sPrg1)
 *  |
 *  ...
 * 
*/
const state sSendACK =
{
    .act = &actSendAck,
    .tran_t = &check_sSendACK,
} ;

/** 
 * State sSendNACK
 * @state: Release Receive for new Uart Msg
 * @action: 
 *  - Send no Acknowledge
 *  - Free Receiver
 * @transition:
 * ------------------------------------------------------------> (sPrgShow)
 * 
*/
const state sSendNACK =
{
    .act = &actSendNAck,
    .tran_t = &check_sSendNACK,
} ;


/* ------------------ Program Start ------------------ */

/** 
 * State sPrgStart 
 * @state: Start Prg
 * @action: 
 *  - MSG: Program started
 * @transition:
 * ----(!b)-----------------------------------------> (sPrgSwtch)
 *          |
 *          --(current Prg equal to Demo)-----------> (sPrgDemo)
 *          |
 *          ...
 * 
*/
const state sPrgStart =
{
    .act = &actPrgStartMsg,
    .tran_t = &check_sPrgStart,
} ;


/* ------------------ Program Modes ------------------ */
/** 
 * State sPrgDemo 
 * Entry Demo Programm
 */
const state sPrgDemo =
{
    .act = &actDemoEnter,
    .tran_t = &check_bsPrgShow,
} ;

/** 
 * State sPrgPErr 
 * Entry PErr
 */
const state sPrgHEra =
{
    .act = &actHEraEnter,
    .tran_t = &check_bsPrgShow,
} ;
/** 
 * State sPrgPErr 
 * Entry PErr
 */

const state sPrgTFNext =
{
    .act = &actTFNextEnter,
    .tran_t = &check_bsPrgShow,
} ;
/** 
 * State sPrgPErr 
 * Entry PErr
 */

const state sPrgPErr =
{
    .act = &actPErrEnter,
    .tran_t = &check_bsPrgShow,
} ;

/** 
 * State sPrgCErr 
 * state sPrgCErr Entry
 */

const state sPrgCErr =
{
    .act = &actCErrEnter,
    .tran_t = &check_sPrgCErr,
} ;

/** 
 * State sPrgCErr 
 * state sPrgCErrB Base
 */

const state sPrgCErrB =
{
    .act = &actCErrBase,
    .tran_t = &check_sPrgCErrB,
} ;
/** 
 * State sPrgCErr 
 * state sPrgCErr Leaf or create Error
 */

const state sPrgCErrO =
{
    .act = &actCErrOr,
    .tran_t = &check_sPrgCErrO,
} ;
/** 
 * State sPrgCErr 
 * state sPrgCErr Create Error
 */

const state sPrgCErrC =
{
    .act = &actCErrCreate,
    .tran_t = &check_sPrgCErrC,
} ;

/** 
 * Init function for Prg Manager
 */
uint8_t hPrg1_init(void)
{  
    if (prgMng_init(&hPrg1, &sPrgShow)==PRG_MNG_OK)
    {
        return PRG_MNG_OK;
    }
    return PRG_MNG_FAILED;
}



/* Implementation of the Functions --------------------------------------------------------------*/
/* Intern Function Toolset */
/* Setter */
uint8_t setCurPrg(uint8_t prg)
{
    if (prg < _PRG_END)
    {
        curPrg = prg;
        return PRG_MNG_OK;
    }
    return PRG_MNG_FAILED;
}

/* Getter */
uint8_t getCurPrg(void)
{
    return curPrg;
}

/* Others */

const state * getStateOfPrg(uint8_t prg)
{
    switch (getCurPrg())
    {
        case PRG_Demo:
        {
            return &sPrgDemo;
        }
        case PRG_CErr:
        {
            return &sPrgCErr;
        }
        case PRG_HEra:
        {
            return &sPrgHEra;
        }
        case PRG_TNext:
        {
            return &sPrgTFNext;
        }
        case PRG_PErr:
        {
            return &sPrgPErr;
        }
        default:
        {
            return &sPrgSwtch;
        }
    }
} 

/* Actions */
/* Program Selection - Button */
void actPrintCurrentPrg(void)
{
    //sendUartMsg("DBG 13-0\n",sizeof("DBG 13-1\n"));
    switch (getCurPrg())
    {
        case PRG_CErr:
        {
            sendUartMsg("PRG_CErr\n",sizeof("PRG_CErr\n"));
            break;
        }
        case PRG_PErr:
        {
            sendUartMsg("PRG_PErr\n",sizeof("PRG_PErr\n"));
            break;
        }
        case PRG_HEra:
        {
            sendUartMsg("PRG_HEra\n",sizeof("PRG_HEra\n"));
            break;
        }
        case PRG_TNext:
        {
            sendUartMsg("PRG_TNext\n",sizeof("PRG_TNext\n"));
            break;
        }
        case PRG_Demo:
        {
            sendUartMsg("PRG_Demo\n",sizeof("PRG_Demo\n"));
            break;
        }
        default:
        {
            sendUartMsg("Error: actPrintCurrentPrg\n",sizeof("Error: actPrintCurrentPrg\n"));
            DBG_Error_Handler( MOD_PRG_RUNNER_H , FNC_actPrintCurrentPrg , RSN_UNEXPECTED_VALUE , 0 );
        }
    }
}
void actNextPrg(void)
{
    //sendUartMsg("DBG 13-2\n",sizeof("DBG 13-1\n"));
    uint8_t curPrg = getCurPrg();
    if (curPrg < _PRG_END-1)
    {
        setCurPrg(curPrg+1);    
    }
    else
    {
        setCurPrg(0);
    }
}
void actSetTimer(void)
{
    //sendUartMsg("DBG 13-1\n",sizeof("DBG 13-1\n"));
    setTimer(20);
}

/* Program Selection - UART */
void actSendAck(void)
{
    setCurPrg(getUartCmd());
    sendUartMsg("ACK\n",sizeof("ACK\n"));
    resetUartCom();
}
void actSendNAck(void)
{
    sendUartMsg("NACK\n",sizeof("NACK\n"));
    resetUartCom();
}

/* Program Start */
void actPrgStartMsg(void)
{
    //sendUartMsg("DBG 13-3\n",sizeof("DBG 13-1\n"));
    sendUartMsg("Start Program Mode\n" , sizeof("Start Program Mode\n") );
}

/* Program Demo */
void actDemoEnter(void)
{
    static uint8_t counter = 0;
    sendUartMsg("This is just a Demo Message\n",sizeof("This is just a Demo Message\n"));
    counter++;
    switch (counter%7)
    {
    case 1:
        sendUartMsg("Hallo Vishal\n",sizeof("Hallo Vishal\n"));
        break;
    case 2:
        sendUartMsg("Hallo Mani\n",sizeof("Hallo Mani\n"));
        break;
    case 3:
        sendUartMsg("Hallo Patrick\n",sizeof("Hallo Patrick\n"));
        break;
    case 4:
        sendUartMsg("Hallo Max\n",sizeof("Hallo Max\n"));
        break;
    case 5:
        sendUartMsg("Hallo Sven\n",sizeof("Hallo Sven\n"));
        break;
    case 6:
        sendUartMsg("Hallo Swati\n",sizeof("Hallo Swati\n"));
        break;
    
    default:
        sendUartMsg("Hallo Christina\n",sizeof("Hallo Christina\n"));
        break;
    }
}
/* Program Print All Errors*/
void actHEraEnter(void)
{
    sendUartMsg("Erase hole HNCS Flash Block\n",sizeof("Erase hole HNCS Flash Block\n"));
    if ( flash_eraseHNCS() == PRG_MNG_FAILED )
    {
        sendUartMsg("Couldn\'t erase the HNCS Flash Block.\n",sizeof("Couldn\'t erase the HNCS Flash Block.\n"));
    }
}
/* Program Print All Errors*/
void actTFNextEnter(void)
{
    sendUartMsg("Test - Write Next Element\n",sizeof("Test - Write Next Element\n"));
    if ( flash_writeNext() == PRG_MNG_FAILED )      
    {
        sendUartMsg("Couldn\'t write the next Block.\n",sizeof("Couldn\'t write the next Block.\n"));
    }
}
/* Program Print All Errors*/
void actPErrEnter(void)
{
    sendUartMsg("Entered the Print All Errors Mode\n",sizeof("Entered the Print All Errors Mode\n"));
    if ( printErrorData() == PRG_MNG_FAILED )
    {
        sendUartMsg("No Error available\n",sizeof("No Error available\n"));
    }
}
/* Program Create Errors */
void actCErrEnter(void)
{
    sendUartMsg("Entered the Error Create Mode\n",sizeof("Entered the Error Create Mode\n"));
}
void actCErrBase(void)
{
    sendUartMsg("Press B1 to create one more Error.\n",sizeof("Press B1 to create one more Error.\n"));
    sendUartMsg("Hold B1 to leaf this mode.\n",sizeof("Hold B1 to leaf this mode.\n"));
}
void actCErrOr(void)
{
    setTimer(20);
}
void actCErrCreate(void)
{
    static uint8_t counter = 0;
    sendUartMsg("Creating Error ...\n",sizeof("Creating Error ...\n"));
    if (writeError(counter,counter+1,counter+2,counter+3) != PRG_MNG_OK)
    {
        sendUartMsg("Can't write an Error, currently in worg mode.\n",sizeof("Can't write an Error, currently in worg mode.\n"));
    }
    counter++;
}

const state * check_sPrgShow(void)
{
    const state * res = NULL;
    if ( uartCmdIsAvailable() == PRG_MNG_OK )
    {
        //sendUartMsg("DBG 13-6\n",sizeof("DBG 13-1\n"));
        res = &sUartCmd;
    }else if( b1Pre() )
    {
        res = &sPrgSwSh;
    }
    return res;
}
const state * check_sPrgSwtch(void)
{
    const state * res = NULL;
    res = &sPrgShow;
    return res;
}
const state * check_sPrgSwSh(void)
{
    const state * res = NULL;
    if ( b1Rel() )
    {
        res = &sPrgSwtch;
    }else if( timerEq0() )
    {
        res = &sPrgStart;
    }
    return res;
}
/* Uart Transitions: */
const state * check_sUartCmd(void)
{
    const state * res = NULL;
    //sendUartMsg("DBG 13-1\n",sizeof("DBG 13-1\n"));
    if (getUartCmd() < _PRG_END )
    {
        res = &sSendACK;
    }
    else
    {
        res = &sSendNACK;
    }
    return res;
}
const state * check_sSendACK(void)
{
    const state * res = NULL;
    res = getStateOfPrg(getCurPrg());
    return res;
}
const state * check_sSendNACK(void)
{
    return &sPrgShow;
}


const state * check_sPrgStart(void)
{
    const state * res = NULL;
    if ( b1Rel() )
    {
        res = getStateOfPrg(getCurPrg());
    }
    return res;
}
const state * check_bsPrgShow(void)
{
    const state * res = NULL;
    res = &sPrgShow;
    return res;
}
const state * check_sPrgCErr(void)
{
    const state * res = NULL;
    if ( b1Rel() )
    {
        res = &sPrgCErrB;
    }
    return res;
}
const state * check_sPrgCErrB(void)
{
    const state * res = NULL;
    if ( b1Pre() )
    {
        res = &sPrgCErrO;
    }
    return res;
}
const state * check_sPrgCErrO(void)
{
    const state * res = NULL;
    if ( b1Rel() )
    {
        res = &sPrgCErrC;
    }
    else if ( timerEq0() )
    {
        res = &sPrgShow;
    }
    return res;
}
const state * check_sPrgCErrC(void)
{
    const state * res = NULL;
    res = &sPrgCErrB;
    return res;
}

/* Conditions*/
uint8_t b1Pre(void)
{
    return (getButtonState()==0U);
}
uint8_t b1Rel(void)
{
    return (getButtonState()==1U);
}
uint8_t timerEq0(void)
{
    return (getPrgTimer0Value()==0U);
}
uint8_t timerGt0(void)
{
    return (getPrgTimer0Value()>0U);
}
uint8_t b1PreAndTimerEq0(void)
{
    if (getButtonState() == 0U  &&  !getPrgTimer0Value() )
    {
        return 1;
    }
    return 0;
}
uint8_t b1PreAndTimerGt0(void)
{
    if (getButtonState() == 0U  &&  getPrgTimer0Value() > 0U )
    {
        return 1;
    }
    return 0;
}
uint8_t b1RelAndTimerGt0(void)
{
    if (getButtonState() == 1  &&  getPrgTimer0Value() > 0 )
    {
        return 1;
    }
    return 0;
}
uint8_t b1RelAndTimerEq0(void)
{
    if (getButtonState() == 1  &&  !getPrgTimer0Value() )
    {
        return 1;
    }
    return 0;
}
uint8_t prgEqDemo(void)
{
    if (  getCurPrg() == PRG_Demo )
    {
        return 1;
    }
    return 0;
}
uint8_t prgEqHEra(void)
{
    if ( getCurPrg() == PRG_HEra)
    {
        return 1;
    }
    return 0;
}
uint8_t prgEqTNext(void)
{
    if ( getCurPrg() == PRG_TNext)
    {
        return 1;
    }
    return 0;
}
uint8_t prgEqCErr(void)
{
    if ( getCurPrg() == PRG_CErr)
    {
        return 1;
    }
    return 0;
}
uint8_t prgEqPErr(void)
{
    if ( getCurPrg() == PRG_PErr)
    {
        return 1;
    }
    return 0;
}


#endif