/**
 *  \file       sim808.c
 *  \brief      Implementation of sim808 control.
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
#include "sim808.h"
#include "rkh.h"
#include "sapi.h"
#include "serial.h"

/* ----------------------------- Local macros ------------------------------ */
/* ------------------------------- Constants ------------------------------- */

static const Sim808PinConfig_t sim808PinConfig[] = {
        { SIM_808_A, ENET_TXEN, ENET_RXD1, ENET_MDC },
        { SIM_808_B, GPIO0, GPIO2, LCDEN },
};

/* ---------------------------- Local data types --------------------------- */
/* ---------------------------- Global variables --------------------------- */
/* ---------------------------- Local variables ---------------------------- */



/* ----------------------- Local function prototypes ----------------------- */
/* ---------------------------- Local functions ---------------------------- */
/* ---------------------------- Global functions --------------------------- */
void sim808Init(Sim808_t sim808){

    if(sim808 >= SIM_808_COUNT)
        return;

    Sim808PinConfig_t pinConfig = sim808PinConfig[sim808];

    sim808SetControlPin(sim808, SIM_808_SLEEP, false);
    gpioConfig( pinConfig.sleepPin, GPIO_OUTPUT );

    sim808SetControlPin(sim808, SIM_808_PWRKEY, false);
    gpioConfig( pinConfig.pwrkeyPin, GPIO_OUTPUT );

    sim808SetControlPin(sim808, SIM_808_PWRKEY, false);

    switch(sim808) {
        case SIM_808_A:
            serialInit(UART_SIM_808_A);
            break;

        case SIM_808_B:
            serialInit(UART_SIM_808_B);
            break;

        default:
            break;
    }
}

void sim808SetControlPin( Sim808_t sim808,  Sim808ControlPin_t controlPin, bool_t data){

    Sim808PinConfig_t pinConfig = sim808PinConfig[sim808];

    switch(controlPin) {
        case SIM_808_SLEEP:
            gpioWrite( pinConfig.sleepPin,data );
            break;

        case SIM_808_PWRKEY:
            gpioWrite( pinConfig.pwrkeyPin,data );
            break;

        case SIM_808_VC:
            if(data){
                gpioWrite( pinConfig.vcPin, false );
                gpioConfig( pinConfig.vcPin, GPIO_OUTPUT );
            } else {
                gpioConfig( pinConfig.vcPin, GPIO_INPUT );
                gpioWrite( pinConfig.vcPin, true );
            }
            break;

        default:
            break;
    }
}

bool_t sim808GetControlPin( Sim808_t sim808,  Sim808ControlPin_t controlPin){

    Sim808PinConfig_t pinConfig = sim808PinConfig[sim808];

    switch(controlPin) {
        case SIM_808_SLEEP:
            return gpioRead( pinConfig.sleepPin);

        case SIM_808_PWRKEY:
            return gpioRead( pinConfig.pwrkeyPin );

        case SIM_808_VC:
            return !gpioRead( pinConfig.vcPin );

        default:
            break;
    }

    return false;
}

/* ------------------------------ End of file ------------------------------ */
