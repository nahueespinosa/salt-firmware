/**
 *  \file       signals.h
 *  \brief      Event signal definitions.
 */

/* -------------------------- Development history -------------------------- */
/*
 *  2018.05.02  DaBa  v1.0.00  Initial version
 *  2018.05.11  LeFr  v1.0.01
 */

/* -------------------------------- Authors -------------------------------- */
/*
 *  DaBa  Dario Bali�a db@vortexmakes.com
 *  LeFr  Leandro Francucci lf@vortexmakes.com
 */

/* --------------------------------- Notes --------------------------------- */
/* --------------------------------- Module -------------------------------- */
#ifndef __SIGNALS_H__
#define __SIGNALS_H__

/* ----------------------------- Include files ----------------------------- */
#include "rkh.h"

/* ---------------------- External C language linkage ---------------------- */
#ifdef __cplusplus
extern "C" {
#endif

/* --------------------------------- Macros -------------------------------- */
/* -------------------------------- Constants ------------------------------ */
/* ................................ Signals ................................ */
enum Signals
{
	evOpen,             // 00
	evClose,            // 01
    evCmd,              // 02
    evResponse,         // 03
    evNoResponse,       // 04
    evURC,              // 05
	evTimeout,          // 06
    evRegTimeout,       // 07
    evToutWaitResponse, // 08
    evOk,               // 09
    evError,            // 10
    evToutDelay,        // 11
    evSimReady,         // 12
    evSimPin,           // 13
    evSimError,         // 14
    evNoReg,            // 15
    evReg,              // 16
    evIP,               // 17
    evIPInitial,        // 18
    evIPStart,          // 19
    evIPStatus,         // 20
    evIPGprsAct,        // 21
    evConnecting,       // 22
    evClosed,           // 23
    evConnected,        // 24
    evSend,             // 25
    evSendFail,         // 26
    evSent,             // 27
    evRecv,             // 28
    evRecvFail,         // 29
    evReceived,         // 30
    evNetConnected,     // 31
    evNetDisconnected,  // 32
    evDisconnected,     // 33
	evTerminate,        // 34         /* press the key escape on the keyboard */
	evConnRefused,      // 35
	evWaitConnectTout,  // 36
	evWaitPublishTout,  // 37
	evWaitSyncTout,     // 38
    evConnAccepted,     // 39
    evActivate,         // 40
    evDeactivate,       // 41
    evSyncTout,         // 42
    evNetClockSync,     // 43
    evLocalTime,        // 44
    evUnlocked,         // 45
    evImei,             // 46
    evOper,             // 47
    evSigLevel,         // 48
	evRestart,          // 49
    evInitEnd,          // 50
    evGps,              // 51
};
typedef enum Signals Signals;

/* ------------------------------- Data types ------------------------------ */
/* -------------------------- External variables --------------------------- */
/* -------------------------- Function prototypes -------------------------- */
/* -------------------- External C language linkage end -------------------- */
#ifdef __cplusplus
}
#endif

/* ------------------------------ Module end ------------------------------- */
#endif

/* ------------------------------ End of file ------------------------------ */
