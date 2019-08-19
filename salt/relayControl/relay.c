/**
 *  \file       relay.h
 *  \brief      Implementation of relay control.
 */

/* -------------------------- Development history -------------------------- */
/*
 *  2018.08.17  IMD  v1.0.01  Initial version
 */

/* -------------------------------- Authors -------------------------------- */
/*
 *  IMD  Ivan Mariano Di Vito      divitoivan@gmail.com
 */

/* --------------------------------- Notes --------------------------------- */
/* ----------------------------- Include files ----------------------------- */
#include "rkh.h"
#include "relay.h"


/* ----------------------------- Local macros ------------------------------ */
/* ------------------------------- Constants ------------------------------- */


/* ---------------------------- Local data types --------------------------- */
/* ---------------------------- Global variables --------------------------- */
/* ---------------------------- Local variables ---------------------------- */
onRelayErrorCb_t onRelayErrorCb;


/* ----------------------- Local function prototypes ----------------------- */
/* ---------------------------- Local functions ---------------------------- */
/* ---------------------------- Global functions --------------------------- */
void relayInit(onRelayErrorCb_t errorCb){
    onRelayErrorCb = errorCb;
    relayInitBsp();
}

void relaySetAlEnable(rbool_t enable){
    relaySetActivated(ctEn, enable);
    relaySetActivated(ctDis, enable);
    relaySetActivated(feEn, enable);
    relaySetActivated(feDis, enable);
}
void relaySetCTActive(rbool_t active){
    if(active && relayGetActivated(ctEn) && relayGetActivated(ctDis)){
        relaySetActivated(ctAct, RKH_TRUE);
    } else {
        relaySetActivated(ctAct, RKH_FALSE);
    }
}
void relaySetFEActive(rbool_t active){
    if(active && relayGetActivated(feEn) && relayGetActivated(feDis)){
        relaySetActivated(feAct, RKH_TRUE);
    } else {
        relaySetActivated(feAct, RKH_FALSE);
    }
}
void relayUpdate(){
    for (int i = 0; i < NUM_RELAY; ++i) {
        if(relayGetActivated(i) != relayReadStatus(i)){
            onRelayErrorCb(i);
        }
    }
}


/* ------------------------------ End of file ------------------------------ */