/**
 *  \file       modcmdSim900.c
 *  \brief      Implementation of module command abstraction.
 */

/* -------------------------- Development history -------------------------- */
/*
 *  2018.05.07  LeFr  v1.0.00  Initial version
 *  2018.05.15  DaBa  v1.0.01  sync
 */

/* -------------------------------- Authors -------------------------------- */
/*
 *  LeFr  Leandro Francucci lf@vortexmakes.com
 *  DaBa  Darío Baliña      db@vortexmakes.com
 */

/* --------------------------------- Notes --------------------------------- */
/* ----------------------------- Include files ----------------------------- */
#include "rkh.h"
#include "rkhfwk_dynevt.h"
#include "modcmd.h"
#include "modmgr.h"
#include "conmgr.h"
#include "sim900parser.h"
#include "signals.h"
#include <string.h>
#include <stdio.h>
#include <bsp.h>

/* ----------------------------- Local macros ------------------------------ */
/* ------------------------------- Constants ------------------------------- */
/* ---------------------------- Local data types --------------------------- */
typedef struct CmdTbl CmdTbl;
struct CmdTbl
{
    ModCmd sync;
    ModCmd initStr;
    ModCmd getPinStatus;
    ModCmd setPin;
    ModCmd getRegStatus;
    ModCmd enableNetTime;
    ModCmd getImei;
    ModCmd cipShutdown;
    ModCmd setManualGet;
    ModCmd getLocalTime;
    ModCmd getOper;
    ModCmd setAPN;
    ModCmd startGPRS;
    ModCmd requestIP;
    ModCmd getConnStatus;
    ModCmd connect;
    ModCmd disconnect;
    ModCmd sendDataRequest;
    ModCmd sendData;
    ModCmd readData;
    ModCmd errorReport;
    ModCmd gpsPowerOn;
    ModCmd gpsPowerOff;
    ModCmd gpsGet;
};



/* ---------------------------- Global variables --------------------------- */
/* ---------------------------- Local variables ---------------------------- */
static const CmdTbl cmdTbl =
{
    {RKH_INIT_STATIC_EVT(evCmd), 
     "AT\r\n", 
     RKH_TIME_MS(2000), RKH_TIME_MS(100)},

    {RKH_INIT_STATIC_EVT(evCmd), 
     "ATE1+CREG=1\r\n",
     RKH_TIME_MS(5000), RKH_TIME_MS(100)},

    {RKH_INIT_STATIC_EVT(evCmd), 
     "AT+CPIN?\r\n", 
     RKH_TIME_MS(1500), RKH_TIME_MS(100)},

    {RKH_INIT_STATIC_EVT(evCmd), 
     "AT+CPIN=%d\r\n", 
     RKH_TIME_MS(500), RKH_TIME_MS(100)},

    {RKH_INIT_STATIC_EVT(evCmd), 
     "AT+CREG?;+CSQ\r\n", 
     RKH_TIME_MS(300), RKH_TIME_MS(500)},

    {RKH_INIT_STATIC_EVT(evCmd), 
     "AT+CLTS=1;+COPS=3,2\r\n",
     RKH_TIME_MS(300), RKH_TIME_MS(100)},

    {RKH_INIT_STATIC_EVT(evCmd), 
     "AT+GSN\r\n",
     RKH_TIME_MS(500), RKH_TIME_MS(100)},

    {RKH_INIT_STATIC_EVT(evCmd), 
     "AT+CIPSHUT\r\n",
     RKH_TIME_MS(3000), RKH_TIME_MS(200)},

    {RKH_INIT_STATIC_EVT(evCmd), 
#ifdef GPRS_QUICK_SEND
     "AT+CIPRXGET=1;+CIPQSEND=1\r\n",
#else
     "AT+CIPRXGET=1\r\n",
#endif
     RKH_TIME_MS(300), RKH_TIME_MS(100)},

    {RKH_INIT_STATIC_EVT(evCmd), 
     "AT+CCLK?\r\n",
     RKH_TIME_MS(300), RKH_TIME_MS(100)},

    {RKH_INIT_STATIC_EVT(evCmd), 
     "AT+COPS?\r\n",
     RKH_TIME_MS(2000), RKH_TIME_MS(100)},

    {RKH_INIT_STATIC_EVT(evCmd), 
     "AT+CSTT=\"%s\",\"%s\",\"%s\"\r\n", 
     RKH_TIME_MS(1000), RKH_TIME_MS(500)},

    {RKH_INIT_STATIC_EVT(evCmd), 
     "AT+CIICR\r\n", 
     RKH_TIME_MS(30000), RKH_TIME_MS(200)},

    {RKH_INIT_STATIC_EVT(evCmd), 
     "AT+CIFSR\r\n", 
     RKH_TIME_MS(1000), RKH_TIME_MS(100)},

    {RKH_INIT_STATIC_EVT(evCmd), 
     "AT+CIPSTATUS;+CSQ\r\n", 
     RKH_TIME_MS(100), RKH_TIME_MS(100)},

    {RKH_INIT_STATIC_EVT(evCmd), 
     "AT+CIPSTART=\"%s\",\"%s\",\"%s\"\r\n", 
     RKH_TIME_MS(1000), RKH_TIME_MS(300)},

    {RKH_INIT_STATIC_EVT(evCmd), 
     "AT+CIPCLOSE\r\n", 
     RKH_TIME_MS(1000), RKH_TIME_MS(100)},

    {RKH_INIT_STATIC_EVT(evCmd), 
     "AT+CIPSEND=%d\r\n", 
     RKH_TIME_MS(3000), RKH_TIME_MS(100)},

    {RKH_INIT_STATIC_EVT(evCmd), 
     "\x1A\r\n", 
#ifdef GPRS_QUICK_SEND
     RKH_TIME_MS(10000), RKH_TIME_MS(100)},
#else
     RKH_TIME_MS(1000), RKH_TIME_MS(100)},
#endif

    {RKH_INIT_STATIC_EVT(evCmd), 
     "AT+CIPRXGET=2,1024\r\n", 
     RKH_TIME_MS(3000), RKH_TIME_MS(100)},
    {RKH_INIT_STATIC_EVT(evCmd),
     "AT+CMEE=1\r\n",
     RKH_TIME_MS(5000), RKH_TIME_MS(100)},
    {RKH_INIT_STATIC_EVT(evCmd),
     "AT+CGPSPWR=1;+CGPSOUT=32\r\n",
     //"AT+CGPSPWR=1\r\n",
     RKH_TIME_MS(5000), RKH_TIME_MS(500)},
    {RKH_INIT_STATIC_EVT(evCmd),
     "AT+CGPSOUT=0;+CGPSPWR=0\r\n",
     RKH_TIME_MS(5000), RKH_TIME_MS(100)},
    {RKH_INIT_STATIC_EVT(evCmd),
     "AT+CGPSINF=32\r\n",
     RKH_TIME_MS(5000), RKH_TIME_MS(100)},
};



/* ----------------------- Local function prototypes ----------------------- */
/* ---------------------------- Local functions ---------------------------- */

static void
doSearch(SSP* ssp, unsigned char c)
{
    sim808parser_doSearch(ssp, c);
}

postFIFOEvtCmd(RKH_SMA_T* ao, RKH_SMA_T* sender, ModMgrEvt *pe, const ModCmd *pc, unsigned char *data, ruint nData)
{
    pe->data = data;
    pe->nData = nData;
    pe->args.fmt = pc->fmt;
    pe->args.waitResponseTime = pc->waitResponseTime;
    pe->args.interCmdTime = pc->interCmdTime;

    RKH_SMA_POST_FIFO(ao, RKH_UPCAST(RKH_EVT_T, pe), sender);
}

static void
sendModCmd_noArgs(RKH_SMA_T* ao, RKH_SMA_T* sender, const ModCmd *p)
{
    ModMgrEvt *evtCmd;

    evtCmd = RKH_ALLOC_EVT(ModMgrEvt, evCmd, sender);

    strncpy(evtCmd->cmd, p->fmt, MODMGR_MAX_SIZEOF_CMDSTR);

    postFIFOEvtCmd(ao, sender, evtCmd, p, NULL, 0);
}

static void
sendModCmd_rui16(RKH_SMA_T* ao, RKH_SMA_T* sender, const ModCmd *p, rui16_t arg)
{
    ModMgrEvt *evtCmd;

    evtCmd = RKH_ALLOC_EVT(ModMgrEvt, evCmd, sender);

    snprintf(evtCmd->cmd, MODMGR_MAX_SIZEOF_CMDSTR, p->fmt, arg);

    postFIFOEvtCmd(ao, sender, evtCmd, p, NULL, 0);
}

static void
sendModCmd_3StrArgs(RKH_SMA_T* ao, RKH_SMA_T* sender, const ModCmd *p, char *s1, char *s2, char *s3)
{
    ModMgrEvt *evtCmd;

    evtCmd = RKH_ALLOC_EVT(ModMgrEvt, evCmd, sender);
    
    snprintf(evtCmd->cmd, MODMGR_MAX_SIZEOF_CMDSTR, p->fmt, s1, s2, s3);

    postFIFOEvtCmd(ao, sender, evtCmd, p, NULL, 0);
}

/* ---------------------------- Global functions --------------------------- */
ModCmdRcvHandler
ModCmd_init(SSP* parser)
{
    RKH_SR_ALLOC();

    RKH_ENTER_CRITICAL_();
  	ssp_init(parser, &rootCmdParser);
    RKH_EXIT_CRITICAL_();
    return &doSearch;
}

void 
ModCmd_sync(RKH_SMA_T* ao, RKH_SMA_T* sender)
{
    sendModCmd_noArgs(ao, sender, &cmdTbl.sync);
}

void 
ModCmd_initStr(RKH_SMA_T* ao, RKH_SMA_T* sender)
{
    sendModCmd_noArgs(ao, sender, &cmdTbl.initStr);
}

void 
ModCmd_getPinStatus(RKH_SMA_T* ao, RKH_SMA_T* sender)
{
    sendModCmd_noArgs(ao, sender, &cmdTbl.getPinStatus);
}

void 
ModCmd_setPin(RKH_SMA_T* ao, RKH_SMA_T* sender, rui16_t pin)
{
    sendModCmd_rui16(ao, sender, &cmdTbl.setPin, pin);
}

void 
ModCmd_getRegStatus(RKH_SMA_T* ao, RKH_SMA_T* sender)
{
    sendModCmd_noArgs(ao, sender, &cmdTbl.getRegStatus);
}

void 
ModCmd_enableNetTime(RKH_SMA_T* ao, RKH_SMA_T* sender)
{
    sendModCmd_noArgs(ao, sender, &cmdTbl.enableNetTime);
}

void 
ModCmd_getImei(RKH_SMA_T* ao, RKH_SMA_T* sender)
{
    sendModCmd_noArgs(ao, sender, &cmdTbl.getImei);
}

void 
ModCmd_cipShutdown(RKH_SMA_T* ao, RKH_SMA_T* sender)
{
    sendModCmd_noArgs(ao, sender, &cmdTbl.cipShutdown);
}

void 
ModCmd_setManualGet(RKH_SMA_T* ao, RKH_SMA_T* sender)
{
    sendModCmd_noArgs(ao, sender, &cmdTbl.setManualGet);
}

void 
ModCmd_getLocalTime(RKH_SMA_T* ao, RKH_SMA_T* sender)
{
    sendModCmd_noArgs(ao, sender, &cmdTbl.getLocalTime);
}

void 
ModCmd_getOper(RKH_SMA_T* ao, RKH_SMA_T* sender)
{
    sendModCmd_noArgs(ao, sender, &cmdTbl.getOper);
}

void 
ModCmd_setupAPN(RKH_SMA_T* ao, RKH_SMA_T* sender, char *apn, char *usr, char *nm)
{
    sendModCmd_3StrArgs(ao, sender, &cmdTbl.setAPN, apn, usr, nm);
}

void 
ModCmd_startGPRS(RKH_SMA_T* ao, RKH_SMA_T* sender)
{
    sendModCmd_noArgs(ao, sender, &cmdTbl.startGPRS);
}

void 
ModCmd_requestIP(RKH_SMA_T* ao, RKH_SMA_T* sender)
{
    sendModCmd_noArgs(ao, sender, &cmdTbl.requestIP);
}

void 
ModCmd_getConnStatus(RKH_SMA_T* ao, RKH_SMA_T* sender)
{
    sendModCmd_noArgs(ao, sender, &cmdTbl.getConnStatus);
}

void 
ModCmd_connect(RKH_SMA_T* ao, RKH_SMA_T* sender, char *prot, char *dmn, char *port)
{
    sendModCmd_3StrArgs(ao, sender, &cmdTbl.connect, prot, dmn, port);
}

void 
ModCmd_disconnect(RKH_SMA_T* ao, RKH_SMA_T* sender)
{
    sendModCmd_noArgs(ao, sender, &cmdTbl.disconnect);
}

void
ModCmd_sendDataRequest(RKH_SMA_T* ao, RKH_SMA_T* sender, rui16_t len)
{
    sendModCmd_rui16(ao, sender, &cmdTbl.sendDataRequest, len);
}

void
ModCmd_sendData(RKH_SMA_T* ao, RKH_SMA_T* sender, unsigned char *buf, ruint size)
{
    ModMgrEvt *evtCmd;
    const ModCmd *p;

    p = &cmdTbl.sendData;

    evtCmd = RKH_ALLOC_EVT(ModMgrEvt, evCmd, sender);

    evtCmd->data = buf;
    evtCmd->nData = size;

    postFIFOEvtCmd(ao, sender, evtCmd, p, buf, size);
}

void
ModCmd_readData(RKH_SMA_T* ao, RKH_SMA_T* sender)
{
    sendModCmd_noArgs(ao, sender, &cmdTbl.readData);
}

char *
ModCmd_endOfXmitStr()
{
    return (char *)cmdTbl.sendData.fmt;
}

void ModCmd_errorReport(RKH_SMA_T* ao, RKH_SMA_T* sender){
    sendModCmd_noArgs(ao, sender, &cmdTbl.errorReport);
}

void
ModCmd_startGPS(RKH_SMA_T* ao, RKH_SMA_T* sender)
{
    sendModCmd_noArgs(ao, sender, &cmdTbl.gpsPowerOn);
}

void
ModCmd_getGPS(RKH_SMA_T* ao, RKH_SMA_T* sender)
{
    sendModCmd_noArgs(ao, sender, &cmdTbl.gpsGet);
}

/* ------------------------------ End of file ------------------------------ */
