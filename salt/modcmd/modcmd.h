/**
 *  \file       modcmd.h
 *  \brief      Specification of module command abstraction.
 */

/* -------------------------- Development history -------------------------- */
/*
 *  2018.05.07  LeFr  v1.0.00  Initial version
 */

/* -------------------------------- Authors -------------------------------- */
/*
 *  LeFr  Leandro Francucci lf@vortexmakes.com
 */

/* --------------------------------- Notes --------------------------------- */
/* --------------------------------- Module -------------------------------- */
#ifndef __MODCMD_H__
#define __MODCMD_H__

/* ----------------------------- Include files ----------------------------- */
#include "rkh.h"
#include "ssp.h"

/* ---------------------- External C language linkage ---------------------- */
#ifdef __cplusplus
extern "C" {
#endif

/* --------------------------------- Macros -------------------------------- */
/* -------------------------------- Constants ------------------------------ */
/* ................................ Signals ................................ */
/* ........................ Declares active object ......................... */
/* ------------------------------- Data types ------------------------------ */
typedef void (*ModCmdRcvHandler)(SSP* parser, unsigned char c);

typedef struct ModCmd ModCmd;
struct ModCmd
{
    RKH_EVT_T evt;
    const char *fmt;            /** contains a format string of AT command */
    RKH_SMA_T * const *aoDest;   /** pointer to SMA that receive the modem */
                                /*  response */
    RKH_TNT_T waitResponseTime; /** timeout waiting for modem response */
    RKH_TNT_T interCmdTime;     /** inter-command delay */
};

/* -------------------------- External variables --------------------------- */
/* -------------------------- Function prototypes -------------------------- */
ModCmdRcvHandler ModCmd_init(SSP* parser);
void ModCmd_sync(RKH_SMA_T* ao);
void ModCmd_initStr(RKH_SMA_T* ao);
void ModCmd_getPinStatus(RKH_SMA_T* ao);
void ModCmd_setPin(RKH_SMA_T* ao, rui16_t pin);
void ModCmd_getRegStatus(RKH_SMA_T* ao);
void ModCmd_enableNetTime(RKH_SMA_T* ao);
void ModCmd_getImei(RKH_SMA_T* ao);
void ModCmd_cipShutdown(RKH_SMA_T* ao);
void ModCmd_setManualGet(RKH_SMA_T* ao);
void ModCmd_getLocalTime(RKH_SMA_T* ao);
void ModCmd_getOper(RKH_SMA_T* ao);
void ModCmd_setupAPN(RKH_SMA_T* ao, char *apn, char *usr, char *nm);
void ModCmd_startGPRS(RKH_SMA_T* ao);
void ModCmd_requestIP(RKH_SMA_T* ao);
void ModCmd_getConnStatus(RKH_SMA_T* ao);
void ModCmd_connect(RKH_SMA_T* ao, char *prot, char *dmn, char *port);
void ModCmd_disconnect(RKH_SMA_T* ao);
void ModCmd_sendDataRequest(RKH_SMA_T* ao, rui16_t len);
void ModCmd_sendData(RKH_SMA_T* ao, unsigned char *buf, ruint size);
void ModCmd_readData(RKH_SMA_T* ao);
char *ModCmd_endOfXmitStr(void);

void ModCmd_errorReport(RKH_SMA_T* ao);
void ModCmd_startGPS(RKH_SMA_T* ao);
void ModCmd_getGPS(RKH_SMA_T* ao);



/* -------------------- External C language linkage end -------------------- */
#ifdef __cplusplus
}
#endif

/* ------------------------------ Module end ------------------------------- */
#endif

/* ------------------------------ End of file ------------------------------ */
