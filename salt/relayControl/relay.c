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
#define ERROR_COUNT_THR 5


/* ---------------------------- Local data types --------------------------- */
/* ---------------------------- Global variables --------------------------- */
/* ---------------------------- Local variables ---------------------------- */
onRelayErrorCb_t onRelayErrorCb;
ri8_t error[NUM_RELAY];

/* ----------------------- Local function prototypes ----------------------- */
/* ---------------------------- Local functions ---------------------------- */
/* ---------------------------- Global functions --------------------------- */
void relayInit(onRelayErrorCb_t errorCb){
    onRelayErrorCb = errorCb;
    relayInitBsp();
    for (int i = 0; i < NUM_RELAY; ++i) {
        error[i] = 0;
    }
}

void relaySetAlEnable(rbool_t enable){
    relaySetActivated(ctEn, enable);
    relaySetActivated(ctDis, enable);
    relaySetActivated(feEn, enable);
    relaySetActivated(feDis, enable);

    relaySetActivated(reg1, enable);
}
void relaySetCTActive(rbool_t active){
    if(active && relayGetActivated(ctEn) && relayGetActivated(ctDis)){
        relaySetActivated(ctAct, RKH_TRUE);

        relaySetActivated(reg2, RKH_TRUE);
    } else {
        relaySetActivated(ctAct, RKH_FALSE);

        relaySetActivated(reg2, RKH_FALSE);
    }

}
void relaySetFEActive(rbool_t active){
    if(active && relayGetActivated(feEn) && relayGetActivated(feDis)){
        relaySetActivated(feAct, RKH_TRUE);

        relaySetActivated(reg3, RKH_TRUE);
    } else {
        relaySetActivated(feAct, RKH_FALSE);

        relaySetActivated(reg3, RKH_FALSE);
    }
}

void relaySetRemoteMode(rbool_t active){
    relaySetActivated(reg4, active);
}

void relayUpdate(){
    for (int i = 0; i < NUM_RELAY; ++i) {
        rbool_t relayStatus = relayReadStatus(i);
        rbool_t relayActivation = relayGetActivated(i);
        if(relayActivation != relayStatus ){
            error[i]++;
            if (error[i] >= ERROR_COUNT_THR){
                error[i] = 0;
                onRelayErrorCb(i);
            }
        } else {
            error[i] = 0;
        }
    }
}


/* ------------------------------ End of file ------------------------------ */