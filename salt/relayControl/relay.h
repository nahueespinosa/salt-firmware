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
    ctEn, ctDis, cdAct,
    reg1, reg2, reg3, reg4,
    NUM_RELAY
}Relay;

typedef struct RelayPinConfig_t{
    Relay relay;
    gpioMap_t actPin;
    gpioMap_t statusPin;
} RelayPinConfig_t;

/* -------------------------- External variables --------------------------- */
/* -------------------------- Function prototypes -------------------------- */
void relayInit(void);
void relaySetActivated(Relay relay, bool activated);
bool relayGetActivated(Relay relay);
bool relayReadStatus(Relay relay);

/* -------------------- External C language linkage end -------------------- */
#ifdef __cplusplus
}
#endif

/* ------------------------------ Module end ------------------------------- */
#endif

/* ------------------------------ End of file ------------------------------ */