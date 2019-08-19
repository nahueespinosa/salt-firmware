/**
 *  \file       pulseCounter.c
 *  \brief      Tachometer pulse counter logic.
 */

/* -------------------------- Development history -------------------------- */
/*
 *  2019.06.17  IMD  v1.0.00  Initial version
 */

/* -------------------------------- Authors -------------------------------- */
/*
 *  IMD  Ivan Mariano Di Vito divitoivan@gmail.com.com
 */

/* --------------------------------- Notes --------------------------------- */
/* ----------------------------- Include files ----------------------------- */
#include "pulseCounter.h"
#include "logic.h"
#include "signals.h"

/* ----------------------------- Local macros ------------------------------ */
/* ------------------------------- Constants ------------------------------- */


/* ---------------------------- Local data types --------------------------- */
/* ---------------------------- Global variables --------------------------- */
/* ---------------------------- Local variables ---------------------------- */
float factor = 0.0;
VelEvt externalVelEvt;
/* ----------------------- Local function prototypes ----------------------- */
/* ---------------------------- Local functions ---------------------------- */

/* ---------------------------- Global functions --------------------------- */
void pulseCounterInit(pulseCount_t errorThr, float velFactor){
    pulseCounterInitBsp(errorThr);
    factor = velFactor;
    externalVelEvt.source = VEL_SOURCE_EXTERNAL;
    RKH_SET_STATIC_EVENT(RKH_UPCAST(RKH_EVT_T, &externalVelEvt), evVelExternal);
}
void pulseCounterUpdate(){
    pulseCount_t pulseCount;
    pulseCounterGet(&pulseCount);
    if(pulseCount != 0){
        externalVelEvt.vel = pulseCount * factor;
    } else {
        externalVelEvt.vel = -1;
    }
    RKH_SMA_POST_FIFO(logic, RKH_UPCAST(RKH_EVT_T,&externalVelEvt), 0);
}

/* ------------------------------ End of file ------------------------------ */
