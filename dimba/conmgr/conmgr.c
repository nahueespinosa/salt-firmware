/**
 *  \file       conmgr.c
 *  \brief      Implementation of connection and protocol manager.
 */

/* -------------------------- Development history -------------------------- */
/*
 *  2018.05.15  DaBa  v1.0.01  Initial version
 *  2018.05.07  LeFr  v1.0.00  Initial version
 */

/* -------------------------------- Authors -------------------------------- */
/*
 *  DaBa  Darío Baliña      db@vortexmakes.com
 *  LeFr  Leandro Francucci lf@vortexmakes.com
 */

/* --------------------------------- Notes --------------------------------- */
/* ----------------------------- Include files ----------------------------- */
#include "rkh.h"
#include "conmgr.h"
#include "modmgr.h"
#include "modcmd.h"
#include "signals.h"
#include "bsp.h"

/* ----------------------------- Local macros ------------------------------ */
/* ......................... Declares active object ........................ */
typedef struct ConMgr ConMgr;

/* ................... Declares states and pseudostates .................... */
RKH_DCLR_BASIC_STATE ConMgr_inactive, ConMgr_sync,
                ConMgr_init, ConMgr_pin, ConMgr_setPin, ConMgr_waitReg,
                    ConMgr_unregistered, ConMgr_failure,
                ConMgr_setManualGet, ConMgr_setAPN, ConMgr_enableGPRS,
                ConMgr_checkIP, ConMgr_waitRetryConfig, ConMgr_connecting,
                ConMgr_idle, ConMgr_waitPrompt, ConMgr_waitOk, ConMgr_receiving, 
                ConMgr_connectError;

RKH_DCLR_COMP_STATE ConMgr_active, ConMgr_initialize, ConMgr_registered,
                    ConMgr_configure, ConMgr_connect, ConMgr_connected,
                    ConMgr_sending;
RKH_DCLR_FINAL_STATE ConMgr_initializeFinal, ConMgr_configureFinal,
                     ConMgr_sendingFinal;
RKH_DCLR_COND_STATE ConMgr_checkSyncTry, ConMgr_checkConfigTry;

/* ........................ Declares initial action ........................ */
static void init(ConMgr *const me, RKH_EVT_T *pe);

/* ........................ Declares effect actions ........................ */
static void open(ConMgr *const me, RKH_EVT_T *pe);
static void initializeInit(ConMgr *const me, RKH_EVT_T *pe);
static void enableUnsolicitedRegStatus(ConMgr *const me, RKH_EVT_T *pe);
static void configureInit(ConMgr *const me, RKH_EVT_T *pe);
static void configTry(ConMgr *const me, RKH_EVT_T *pe);
static void requestIp(ConMgr *const me, RKH_EVT_T *pe);
static void socketOpen(ConMgr *const me, RKH_EVT_T *pe);
static void socketClose(ConMgr *const me, RKH_EVT_T *pe);
static void sendData(ConMgr *const me, RKH_EVT_T *pe);
static void readData(ConMgr *const me, RKH_EVT_T *pe);
static void sendRequest(ConMgr *const me, RKH_EVT_T *pe);
static void flushData(ConMgr *const me, RKH_EVT_T *pe);

/* ......................... Declares entry actions ........................ */
static void sendSync(ConMgr *const me);
static void sendInit(ConMgr *const me);
static void checkPin(ConMgr *const me);
static void setPin(ConMgr *const me);
static void checkReg(ConMgr *const me);
static void unregEntry(ConMgr *const me);
static void setupManualGet(ConMgr *const me);
static void waitRetryConfigEntry(ConMgr *const me);
static void setupAPN(ConMgr *const me);
static void startGPRS(ConMgr *const me);
static void getConnStatus(ConMgr *const me);
static void startReadPollingTimer(ConMgr *const me);

/* ......................... Declares exit actions ......................... */
static void unregExit(ConMgr *const me);

/* ............................ Declares guards ............................ */
rbool_t checkSyncTry(ConMgr *const me, RKH_EVT_T *pe);
rbool_t checkConfigTry(ConMgr *const me, RKH_EVT_T *pe);

/* ........................ States and pseudostates ........................ */
RKH_CREATE_BASIC_STATE(ConMgr_inactive, NULL, NULL, RKH_ROOT, NULL);
RKH_CREATE_TRANS_TABLE(ConMgr_inactive)
    RKH_TRREG(evOpen, NULL, open, &ConMgr_active),
RKH_END_TRANS_TABLE

RKH_CREATE_COMP_REGION_STATE(ConMgr_active, NULL, NULL, RKH_ROOT, 
                             &ConMgr_initialize, NULL,
                             RKH_NO_HISTORY, NULL, NULL, NULL, NULL);
RKH_CREATE_TRANS_TABLE(ConMgr_active)
    RKH_TRREG(evClose, NULL, NULL, &ConMgr_inactive),
RKH_END_TRANS_TABLE

RKH_CREATE_COMP_REGION_STATE(ConMgr_initialize, NULL, NULL, &ConMgr_active, 
                             &ConMgr_sync, initializeInit,
                             RKH_NO_HISTORY, NULL, NULL, NULL, NULL);
RKH_CREATE_TRANS_TABLE(ConMgr_initialize)
    RKH_TRCOMPLETION(NULL, NULL, &ConMgr_unregistered),
RKH_END_TRANS_TABLE

RKH_CREATE_BASIC_STATE(ConMgr_sync, sendSync, NULL, &ConMgr_initialize, NULL);
RKH_CREATE_TRANS_TABLE(ConMgr_sync)
    RKH_TRREG(evOk,         NULL, NULL, &ConMgr_init),
    RKH_TRREG(evNoResponse, NULL, NULL, &ConMgr_checkSyncTry),
RKH_END_TRANS_TABLE

RKH_CREATE_COND_STATE(ConMgr_checkSyncTry);
RKH_CREATE_BRANCH_TABLE(ConMgr_checkSyncTry)
    RKH_BRANCH(checkSyncTry,   NULL,   &ConMgr_sync),
    RKH_BRANCH(ELSE,           NULL,   &ConMgr_failure),
RKH_END_BRANCH_TABLE

RKH_CREATE_BASIC_STATE(ConMgr_init, sendInit, NULL, &ConMgr_initialize, NULL);
RKH_CREATE_TRANS_TABLE(ConMgr_init)
    RKH_TRREG(evOk,         NULL, NULL, &ConMgr_pin),
    RKH_TRREG(evNoResponse, NULL, NULL, &ConMgr_failure),
RKH_END_TRANS_TABLE

RKH_CREATE_BASIC_STATE(ConMgr_pin, checkPin, NULL, &ConMgr_initialize, NULL);
RKH_CREATE_TRANS_TABLE(ConMgr_pin)
    RKH_TRREG(evSimPin,     NULL, NULL, &ConMgr_setPin),
    RKH_TRREG(evSimError,   NULL, NULL, &ConMgr_failure),
    RKH_TRREG(evSimReady,   NULL, NULL, &ConMgr_initializeFinal),
RKH_END_TRANS_TABLE

RKH_CREATE_BASIC_STATE(ConMgr_setPin, setPin, NULL, &ConMgr_initialize, NULL);
RKH_CREATE_TRANS_TABLE(ConMgr_setPin)
    RKH_TRREG(evOk,         NULL, NULL,   &ConMgr_pin),
RKH_END_TRANS_TABLE

RKH_CREATE_COMP_REGION_STATE(ConMgr_registered, NULL, NULL, &ConMgr_active, 
                             &ConMgr_configure, NULL,
                             RKH_NO_HISTORY, NULL, NULL, NULL, NULL);
RKH_CREATE_TRANS_TABLE(ConMgr_registered)
    RKH_TRREG(evNoReg, NULL, NULL,   &ConMgr_unregistered),
RKH_END_TRANS_TABLE

RKH_CREATE_BASIC_STATE(ConMgr_unregistered, unregEntry, unregExit,
                            &ConMgr_active, NULL);
RKH_CREATE_TRANS_TABLE(ConMgr_unregistered)
    RKH_TRREG(evTimeout,  NULL,    NULL, &ConMgr_failure),
    RKH_TRREG(evReg, NULL, NULL,   &ConMgr_registered),
RKH_END_TRANS_TABLE

RKH_CREATE_BASIC_STATE(ConMgr_failure, NULL, NULL, &ConMgr_active, NULL);
RKH_CREATE_TRANS_TABLE(ConMgr_failure)
    RKH_TRREG(evOpen, NULL,  NULL, &ConMgr_active),
RKH_END_TRANS_TABLE

RKH_CREATE_HISTORY_STORAGE(ConMgr_configure);
RKH_CREATE_COMP_REGION_STATE(ConMgr_configure, NULL, NULL, &ConMgr_registered, 
                             &ConMgr_configureHist, NULL,
                             RKH_SHISTORY, NULL, 
                             configureInit, &ConMgr_setManualGet,
                             RKH_GET_HISTORY_STORAGE(ConMgr_configure));
RKH_CREATE_TRANS_TABLE(ConMgr_configure)
    RKH_TRCOMPLETION(NULL, NULL, &ConMgr_connect),
    RKH_TRREG(evNoResponse, NULL, NULL, &ConMgr_checkConfigTry),
RKH_END_TRANS_TABLE

RKH_CREATE_BASIC_STATE(ConMgr_setManualGet, setupManualGet, NULL, 
                                                    &ConMgr_configure, NULL);
RKH_CREATE_TRANS_TABLE(ConMgr_setManualGet)
    RKH_TRREG(evOk,         NULL, NULL, &ConMgr_setAPN),
RKH_END_TRANS_TABLE

RKH_CREATE_BASIC_STATE(ConMgr_setAPN, setupAPN, NULL, 
                                                    &ConMgr_configure, NULL);
RKH_CREATE_TRANS_TABLE(ConMgr_setAPN)
    RKH_TRREG(evOk,         NULL, NULL, &ConMgr_enableGPRS),
RKH_END_TRANS_TABLE

RKH_CREATE_BASIC_STATE(ConMgr_enableGPRS, startGPRS, NULL, 
                                                    &ConMgr_configure, NULL);
RKH_CREATE_TRANS_TABLE(ConMgr_enableGPRS)
    RKH_TRREG(evOk,         NULL, NULL, &ConMgr_checkIP),
RKH_END_TRANS_TABLE

RKH_CREATE_BASIC_STATE(ConMgr_checkIP, getConnStatus, NULL, 
                                                    &ConMgr_configure, NULL);
RKH_CREATE_TRANS_TABLE(ConMgr_checkIP)
    RKH_TRREG(evIPGprsAct,  requestIp,    NULL, &ConMgr_checkIP),
    RKH_TRREG(evIP,         NULL,         NULL, &ConMgr_checkIP),
    RKH_TRREG(evIPInitial,  NULL,         NULL, &ConMgr_checkIP),
    RKH_TRREG(evIPStart,    NULL,         NULL, &ConMgr_checkIP),
    RKH_TRREG(evIPStatus,   NULL,         NULL, &ConMgr_configureFinal),
RKH_END_TRANS_TABLE

RKH_CREATE_COND_STATE(ConMgr_checkConfigTry);
RKH_CREATE_BRANCH_TABLE(ConMgr_checkConfigTry)
    RKH_BRANCH(checkConfigTry,   NULL, &ConMgr_waitRetryConfig),
    RKH_BRANCH(ELSE,           NULL,   &ConMgr_failure),
RKH_END_BRANCH_TABLE

RKH_CREATE_BASIC_STATE(ConMgr_waitRetryConfig, waitRetryConfigEntry, NULL,
                                                    &ConMgr_registered, NULL);
RKH_CREATE_TRANS_TABLE(ConMgr_waitRetryConfig)
    RKH_TRREG(evTimeout,  NULL,    configTry, &ConMgr_configure),
RKH_END_TRANS_TABLE

RKH_CREATE_COMP_REGION_STATE(ConMgr_connect, NULL, NULL, 
                             &ConMgr_registered, &ConMgr_connecting,
                             socketOpen,
                             RKH_NO_HISTORY, NULL, NULL, NULL, NULL);
RKH_CREATE_TRANS_TABLE(ConMgr_connect)
RKH_END_TRANS_TABLE

RKH_CREATE_BASIC_STATE(ConMgr_connecting, NULL, NULL, &ConMgr_connect, NULL);
RKH_CREATE_TRANS_TABLE(ConMgr_connecting)
    RKH_TRINT(evOk,         NULL,  &getConnStatus),
    RKH_TRINT(evConnecting, NULL,  &getConnStatus),
    RKH_TRREG(evConnected,  NULL,  NULL, &ConMgr_connected),
    RKH_TRREG(evClosed,     NULL,  NULL, &ConMgr_connect),
    RKH_TRREG(evNoResponse, NULL, NULL, &ConMgr_connectError),
RKH_END_TRANS_TABLE

RKH_CREATE_COMP_REGION_STATE(ConMgr_connected, NULL, NULL, 
                             &ConMgr_connect, &ConMgr_idle, NULL,
                             RKH_NO_HISTORY, NULL, NULL, NULL, NULL);
RKH_CREATE_TRANS_TABLE(ConMgr_connected)
    RKH_TRINT(evClose, NULL,  &socketClose),
    RKH_TRREG(evDisconnected, NULL,  NULL, &ConMgr_connect),
    RKH_TRREG(evNoResponse, NULL, NULL, &ConMgr_connectError),
RKH_END_TRANS_TABLE

RKH_CREATE_BASIC_STATE(ConMgr_idle, startReadPollingTimer, 
                                            NULL, &ConMgr_connected, NULL);
RKH_CREATE_TRANS_TABLE(ConMgr_idle)
    RKH_TRREG(evSend, NULL,  sendRequest, &ConMgr_sending),
    RKH_TRREG(evRead, NULL, readData, &ConMgr_receiving),
RKH_END_TRANS_TABLE

RKH_CREATE_COMP_REGION_STATE(ConMgr_sending, NULL, NULL, 
                             &ConMgr_connected, &ConMgr_waitPrompt, NULL,
                             RKH_NO_HISTORY, NULL, NULL, NULL, NULL);
RKH_CREATE_TRANS_TABLE(ConMgr_sending)
    RKH_TRCOMPLETION(NULL, NULL, &ConMgr_idle),
RKH_END_TRANS_TABLE

RKH_CREATE_BASIC_STATE(ConMgr_waitPrompt, NULL, NULL, &ConMgr_sending, NULL);
RKH_CREATE_TRANS_TABLE(ConMgr_waitPrompt)
    RKH_TRREG(evOk, NULL,  flushData, &ConMgr_waitOk),
RKH_END_TRANS_TABLE

RKH_CREATE_BASIC_STATE(ConMgr_waitOk, NULL, NULL, &ConMgr_sending, NULL);
RKH_CREATE_TRANS_TABLE(ConMgr_waitOk)
    RKH_TRREG(evOk, NULL,  NULL, &ConMgr_sendingFinal),
RKH_END_TRANS_TABLE

RKH_CREATE_BASIC_STATE(ConMgr_receiving, NULL, NULL, &ConMgr_connected, NULL);
RKH_CREATE_TRANS_TABLE(ConMgr_receiving)
    RKH_TRREG(evOk, NULL,  NULL, &ConMgr_idle),
RKH_END_TRANS_TABLE

RKH_CREATE_BASIC_STATE(ConMgr_connectError, NULL, NULL, &ConMgr_connect, NULL);
RKH_CREATE_TRANS_TABLE(ConMgr_connectError)
    RKH_TRREG(evOpen, NULL,  NULL, &ConMgr_connect),
RKH_END_TRANS_TABLE

/* ............................. Active object ............................. */
struct ConMgr
{
    RKH_SMA_T ao;       /* base structure */
    RKH_TMR_T timer;    /* which is responsible for toggling the LED */
                        /* posting the TIMEOUT signal event to active object */
                        /* 'conMgr' */
    rui8_t retryCount; 
    SendEvt *psend;
};

RKH_SMA_CREATE(ConMgr, conMgr, 1, HCAL, &ConMgr_inactive, init, NULL);
RKH_SMA_DEF_PTR(conMgr);

/* ------------------------------- Constants ------------------------------- */
/* ---------------------------- Local data types --------------------------- */
/* ---------------------------- Global variables --------------------------- */
/* ---------------------------- Local variables ---------------------------- */
/*
 *  Declare and allocate the 'e_tout' event.
 *  The 'e_tout' event with TIMEOUT signal never changes, so it can be
 *  statically allocated just once by means of RKH_ROM_STATIC_EVENT() macro.
 */
static RKH_STATIC_EVENT(e_tout, evToutDelay);
static RKH_ROM_STATIC_EVENT(e_Open, evOpen);

/* ----------------------- Local function prototypes ----------------------- */
/* ---------------------------- Local functions ---------------------------- */
/* ............................ Initial action ............................. */
static void
init(ConMgr *const me, RKH_EVT_T *pe)
{
	(void)pe;

    RKH_TR_FWK_AO(me);
    RKH_TR_FWK_QUEUE(&RKH_UPCAST(RKH_SMA_T, me)->equeue);
    RKH_TR_FWK_STATE(me, &ConMgr_inactive);
    RKH_TR_FWK_STATE(me, &ConMgr_active);
    RKH_TR_FWK_STATE(me, &ConMgr_initialize);
    RKH_TR_FWK_STATE(me, &ConMgr_sync);
	RKH_TR_FWK_STATE(me, &ConMgr_init);
    RKH_TR_FWK_STATE(me, &ConMgr_pin);
    RKH_TR_FWK_STATE(me, &ConMgr_setPin);
    RKH_TR_FWK_STATE(me, &ConMgr_initializeFinal);
    RKH_TR_FWK_STATE(me, &ConMgr_registered);
    RKH_TR_FWK_STATE(me, &ConMgr_unregistered);
    RKH_TR_FWK_STATE(me, &ConMgr_failure);
    RKH_TR_FWK_STATE(me, &ConMgr_configure);
    RKH_TR_FWK_STATE(me, &ConMgr_setManualGet);
    RKH_TR_FWK_STATE(me, &ConMgr_setAPN);
    RKH_TR_FWK_STATE(me, &ConMgr_enableGPRS);
    RKH_TR_FWK_STATE(me, &ConMgr_checkIP);
    RKH_TR_FWK_STATE(me, &ConMgr_checkConfigTry);
    RKH_TR_FWK_STATE(me, &ConMgr_waitRetryConfig);
    RKH_TR_FWK_STATE(me, &ConMgr_configureFinal);
    RKH_TR_FWK_STATE(me, &ConMgr_connect);
    RKH_TR_FWK_STATE(me, &ConMgr_connecting);
    RKH_TR_FWK_STATE(me, &ConMgr_connected);
    RKH_TR_FWK_STATE(me, &ConMgr_idle);
    RKH_TR_FWK_STATE(me, &ConMgr_sending);
    RKH_TR_FWK_STATE(me, &ConMgr_waitPrompt);
    RKH_TR_FWK_STATE(me, &ConMgr_waitOk);
    RKH_TR_FWK_STATE(me, &ConMgr_sendingFinal);
    RKH_TR_FWK_STATE(me, &ConMgr_receiving);
    RKH_TR_FWK_STATE(me, &ConMgr_connectError);
    RKH_TR_FWK_TIMER(&me->timer);
    RKH_TR_FWK_SIG(evOpen);
    RKH_TR_FWK_SIG(evClose);
    RKH_TR_FWK_SIG(evCmd);
	RKH_TR_FWK_SIG(evOk);
	RKH_TR_FWK_SIG(evURC);
    RKH_TR_FWK_SIG(evSimPin);
    RKH_TR_FWK_SIG(evSimError);
    RKH_TR_FWK_SIG(evSimReady);
    RKH_TR_FWK_SIG(evReg);
    RKH_TR_FWK_SIG(evNoReg);
    RKH_TR_FWK_SIG(evIPInitial);
    RKH_TR_FWK_SIG(evIPStart);
    RKH_TR_FWK_SIG(evIPStatus);
    RKH_TR_FWK_SIG(evIPGprsAct);
    RKH_TR_FWK_SIG(evConnecting);
    RKH_TR_FWK_SIG(evClosed);
    RKH_TR_FWK_SIG(evConnected);
    RKH_TR_FWK_SIG(evSend);
    RKH_TR_FWK_SIG(evRead);
    RKH_TR_FWK_SIG(evDisconnected);
    RKH_TR_FWK_SIG(evTerminate);

    RKH_TMR_INIT(&me->timer, &e_tout, NULL);
    me->retryCount = 0;
}

/* ............................ Effect actions ............................. */
static void
open(ConMgr *const me, RKH_EVT_T *pe)
{
    (void)pe;
    (void)me;

    RKH_SMA_POST_FIFO(modMgr, &e_Open, conMgr);
}

static void
initializeInit(ConMgr *const me, RKH_EVT_T *pe)
{
    (void)pe;

    me->retryCount = 0;
}

static void
configureInit(ConMgr *const me, RKH_EVT_T *pe)
{
    (void)pe;

    me->retryCount = 0;
}

static void
configTry(ConMgr *const me, RKH_EVT_T *pe)
{
    (void)pe;

    ++me->retryCount;
}

static void 
requestIp(ConMgr *const me, RKH_EVT_T *pe)
{
    (void)pe;
    (void)me;

    ModCmd_requestIP();
}

static void
socketOpen(ConMgr *const me, RKH_EVT_T *pe)
{
    (void)me;
    (void)pe;

    ModCmd_connect(CONNECTION_PROT, CONNECTION_DOMAIN, CONNECTION_PORT);
}

static void
socketClose(ConMgr *const me, RKH_EVT_T *pe)
{
    (void)me;
    (void)pe;

    ModCmd_disconnect();
}

static void 
readData(ConMgr *const me, RKH_EVT_T *pe)
{
    (void)pe;
    (void)me;

    ModCmd_readData();
}

static void
sendRequest(ConMgr *const me, RKH_EVT_T *pe)
{
    (void)me;

    RKH_FWK_RSV( pe );
    me->psend = RKH_UPCAST(SendEvt, pe);

    ModCmd_sendDataRequest();
}

static void
flushData(ConMgr *const me, RKH_EVT_T *pe)
{
    (void)pe;

    ModCmd_sendData(me->psend->data);
    RKH_FWK_GC(RKH_CAST(RKH_EVT_T, me->psend), me);
}

/* ............................. Entry actions ............................. */
static void
sendSync(ConMgr *const me)
{
    ++me->retryCount;

    ModCmd_sync();
}

static void
sendInit(ConMgr *const me)
{
    (void)me;

    ModCmd_initStr();
}

static void
checkPin(ConMgr *const me)
{
    (void)me;

    ModCmd_getPinStatus();
}

static void
setPin(ConMgr *const me)
{
    (void)me;

    ModCmd_setPin(SIM_PIN_NUMBER);
}

static void 
unregEntry(ConMgr *const me)
{
    ModCmd_getRegStatus();

    RKH_SET_STATIC_EVENT(&e_tout, evTimeout);
    RKH_TMR_ONESHOT(&me->timer, RKH_UPCAST(RKH_SMA_T, me), REGISTRATION_TIME);
}

static void
waitRetryConfigEntry(ConMgr *const me)
{
    RKH_SET_STATIC_EVENT(&e_tout, evTimeout);
    RKH_TMR_ONESHOT(&me->timer, RKH_UPCAST(RKH_SMA_T, me), CONFIG_TRY_DELAY);
}

static void
setupManualGet(ConMgr *const me)
{
    (void)me;

    ModCmd_setManualGet();
}
   
static void
setupAPN(ConMgr *const me)
{
    (void)me;

    ModCmd_setupAPN(CONNECTION_APN, CONNECTION_USER, CONNECTION_PASSWORD);
}
   
static void
startGPRS(ConMgr *const me)
{
    (void)me;

    ModCmd_startGPRS();
}

static void
getConnStatus(ConMgr *const me)
{
    (void)me;

    ModCmd_getConnStatus();
}

static void
startReadPollingTimer(ConMgr *const me)
{
    (void)me;

    RKH_SET_STATIC_EVENT(&e_tout, evRead);
    RKH_TMR_ONESHOT(&me->timer, RKH_UPCAST(RKH_SMA_T, me), READ_POLLING_TIME);
}


/* ............................. Exit actions ............................. */
static void 
unregExit(ConMgr *const me)
{
    rkh_tmr_stop(&me->timer);
}

/* ................................ Guards ................................. */
rbool_t
checkSyncTry(ConMgr *const me, RKH_EVT_T *pe)
{
    (void)pe;
    
    return (me->retryCount < MAX_SYNC_RETRY) ? RKH_TRUE : RKH_FALSE;
}

rbool_t
checkConfigTry(ConMgr *const me, RKH_EVT_T *pe)
{
    (void)pe;
    
    return (me->retryCount < MAX_CONFIG_RETRY) ? RKH_TRUE : RKH_FALSE;
}

/* ---------------------------- Global functions --------------------------- */
/* ------------------------------ End of file ------------------------------ */
