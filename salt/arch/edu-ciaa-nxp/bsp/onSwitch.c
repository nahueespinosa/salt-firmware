/**
 *  \file       onSwitch.c
 *  \brief      on switch driver.
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
#include "onSwitch.h"
#include "buzzer.h"
#include "sapi.h"

/* ----------------------------- Local macros ------------------------------ */
/* ------------------------------- Constants ------------------------------- */

#define SWITCH_DIGITAL_PIN              GPIO8
#define SWITCH_DIGITAL_PIN_GPIO_PORT    2
#define SWITCH_DIGITAL_PIN_GPIO_PIN     8

#define PININT_SWITCH_INDEX             0
#define PININT_SWITCH                   PININTCH(PININT_SWITCH_INDEX)
#define PININT_SWITCH_NVIC_NAME         PIN_INT0_IRQn
#define PININT_IRQ_HANDLER              GPIO0_IRQHandler

/* ---------------------------- Local data types --------------------------- */
/* ---------------------------- Global variables --------------------------- */
/* ---------------------------- Local variables ---------------------------- */
static onSwitchCb_t onSwitchCb = NULL;
/* ----------------------- Local function prototypes ----------------------- */
/* ---------------------------- Local functions ---------------------------- */
/* ---------------------------- Global functions --------------------------- */

void onSwitchInit(onSwitchCb_t cb){

    /* Set Callback */
    onSwitchSetIntCb(cb);

    // Configure On Switch Pin
    gpioConfig( SWITCH_DIGITAL_PIN, GPIO_INPUT_PULLUP );

    // Configure On Switch Pin Interrupt
    Chip_SCU_GPIOIntPinSel(PININT_SWITCH_INDEX, SWITCH_DIGITAL_PIN_GPIO_PORT, SWITCH_DIGITAL_PIN_GPIO_PIN);

    Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININT_SWITCH);
    Chip_PININT_SetPinModeEdge(LPC_GPIO_PIN_INT, PININT_SWITCH);
    Chip_PININT_EnableIntLow(LPC_GPIO_PIN_INT, PININT_SWITCH);
    Chip_PININT_EnableIntHigh(LPC_GPIO_PIN_INT, PININT_SWITCH);


    /* Enable interrupt in the NVIC */
    NVIC_ClearPendingIRQ(PININT_SWITCH_NVIC_NAME);
    NVIC_EnableIRQ(PININT_SWITCH_NVIC_NAME);
}


bool_t onSwitchGet(){
    return !gpioRead(SWITCH_DIGITAL_PIN);
}

void onSwitchSetIntCb( onSwitchCb_t cb ){
    onSwitchCb = cb;
}

void PININT_IRQ_HANDLER(void)
{
    Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININT_SWITCH);
    if(onSwitchCb != NULL)
        (onSwitchCb)(onSwitchGet());
}


/* ------------------------------ End of file ------------------------------ */
