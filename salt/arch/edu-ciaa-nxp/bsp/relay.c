/**
 *  \file       relay.c
 *  \brief      Implementation of Relay control.
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
/* ----------------------------- Include files ----------------------------- */
#include "rkh.h"
#include "sapi.h"
#include "relay.h"

/* ----------------------------- Local macros ------------------------------ */
/* ------------------------------- Constants ------------------------------- */

static const RelayPinConfig_t relayPinConfig[] = {
        { feEn, ENET_TXD1, LCD3 },
        { feDis, ENET_TXD0, LCDRS },
        { feAct, ENET_MDIO, LCD4 },
        { ctEn, GPIO1, GPIO7 },
        { ctDis, LCD1, GPIO5 },
        { cdAct, LCD2, GPIO3 },
        { reg4, T_COL2, T_FIL1 },
        { reg3, T_COL0, T_FIL2 },
        { reg2, T_FIL3, T_FIL0 },
        { reg1, T_COL1, DI7 }, // DI7 CIAA = ISP EDU-CIAA
};

/* ---------------------------- Local data types --------------------------- */
/* ---------------------------- Global variables --------------------------- */
/* ---------------------------- Local variables ---------------------------- */



/* ----------------------- Local function prototypes ----------------------- */
/* ---------------------------- Local functions ---------------------------- */
/* ---------------------------- Global functions --------------------------- */
void relayInit(void){

    for (int i = 0; i < NUM_RELAY; ++i) {
        relaySetActivated( i, false);
        gpioConfig( relayPinConfig[i].actPin, GPIO_OUTPUT );
        gpioConfig( relayPinConfig[i].statusPin, GPIO_INPUT );
    }

}

void relaySetActivated(Relay relay, bool activated){

    gpioWrite(relayPinConfig[relay].actPin,activated);

}

bool relayGetActivated(Relay relay){

    return gpioRead(relayPinConfig[relay].actPin);

}

bool relayReadStatus(Relay relay){

    return !gpioRead(relayPinConfig[relay].statusPin);

}

/* ------------------------------ End of file ------------------------------ */
