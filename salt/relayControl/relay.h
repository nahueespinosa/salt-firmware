/**
 *  \file       relay.h
 *  \brief      Specification of Relay control.
 */

/* -------------------------- Development history -------------------------- */
/*
 *  2019.06.27  IMD  v1.0.00  Initial version
 */

/* -------------------------------- Authors -------------------------------- */
/*
 *  IMD  Ivan Mariano Di Vito       divitoivan@gmail.com
 */

/* --------------------------------- Notes --------------------------------- */
/* --------------------------------- Module -------------------------------- */
#ifndef __RELAY_H__
#define __RELAY_H__

/* ----------------------------- Include files ----------------------------- */
#include "sapi.h"
/* ---------------------- External C language linkage ---------------------- */
#ifdef __cplusplus
extern "C" {
#endif

/* --------------------------------- Macros -------------------------------- */
/* -------------------------------- Constants ------------------------------ */
/* ------------------------------- Data types ------------------------------ */
typedef enum Relay
{
    feEn, feDis, feAct,
    ctEn, ctDis, ctAct,
    reg1, reg2, reg3, reg4,
    NUM_RELAY, NULL_RELAY
}Relay_t;


typedef struct RelayPinConfig_t{
    Relay_t relay;
    gpioMap_t actPin;
    gpioMap_t statusPin;
} RelayPinConfig_t;

typedef void (* onRelayErrorCb_t)(Relay_t relay);
/* -------------------------- External variables --------------------------- */
/* -------------------------- Function prototypes -------------------------- */
void relayInitBsp(void);
void relaySetActivated(Relay_t relay, rbool_t activated);
rbool_t relayGetActivated(Relay_t relay);
rbool_t relayReadStatus(Relay_t relay);

void relayInit(onRelayErrorCb_t errorCb);
void relaySetAlEnable(rbool_t enable);
void relaySetCTActive(rbool_t active);
void relaySetFEActive(rbool_t active);
void relaySetRemoteMode(rbool_t active);
void relayUpdate();
/* -------------------- External C language linkage end -------------------- */
#ifdef __cplusplus
}
#endif

/* ------------------------------ Module end ------------------------------- */
#endif

/* ------------------------------ End of file ------------------------------ */