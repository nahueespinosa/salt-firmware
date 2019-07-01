/**
 *  \file       pulseCounter.c
 *  \brief      Tachometer pulse counter driver.
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
#include "sapi.h"
#include "math.h"

/* ----------------------------- Local macros ------------------------------ */
/* ------------------------------- Constants ------------------------------- */

#define PULSE_A_PIN                         CAN_TD
#define PULSE_A_PIN_GPIO_PORT               5
#define PULSE_A_PIN_GPIO_PIN                9
#define PULSE_A_PININT_SWITCH_INDEX         1
#define PULSE_A_PININT_SWITCH               PININTCH(PULSE_A_PININT_SWITCH_INDEX)
#define PULSE_A_PININT_SWITCH_NVIC_NAME     PIN_INT1_IRQn
#define PULSE_A_PININT_IRQ_HANDLER          GPIO1_IRQHandler

#define PULSE_B_PIN                         CAN_RD
#define PULSE_B_PIN_GPIO_PORT               5
#define PULSE_B_PIN_GPIO_PIN                8
#define PULSE_B_PININT_SWITCH_INDEX         2
#define PULSE_B_PININT_SWITCH               PININTCH(PULSE_B_PININT_SWITCH_INDEX)
#define PULSE_B_PININT_SWITCH_NVIC_NAME     PIN_INT2_IRQn
#define PULSE_B_PININT_IRQ_HANDLER          GPIO2_IRQHandler


/* ---------------------------- Local data types --------------------------- */
/* ---------------------------- Global variables --------------------------- */
/* ---------------------------- Local variables ---------------------------- */
pulseCount_t pulseACounter = 0;
pulseCount_t pulseBCounter = 0;
pulseCount_t pulseErrorThr = 0;
/* ----------------------- Local function prototypes ----------------------- */
/* ---------------------------- Local functions ---------------------------- */

void pulseCounterEnableInt(bool_t enable){
    if(enable){
        NVIC_ClearPendingIRQ(PULSE_A_PININT_SWITCH_NVIC_NAME);
        NVIC_EnableIRQ(PULSE_A_PININT_SWITCH_NVIC_NAME);

        NVIC_ClearPendingIRQ(PULSE_B_PININT_SWITCH_NVIC_NAME);
        NVIC_EnableIRQ(PULSE_B_PININT_SWITCH_NVIC_NAME);
    } else {
        NVIC_DisableIRQ(PULSE_A_PININT_SWITCH_NVIC_NAME);
        NVIC_DisableIRQ(PULSE_B_PININT_SWITCH_NVIC_NAME);
    }
}

/* ---------------------------- Global functions --------------------------- */

void pulseCounterInit(pulseCount_t errorThr){

    pulseErrorThr = errorThr;

    // Configure Pulse Pins
    gpioConfig( PULSE_A_PIN, GPIO_INPUT );
    gpioConfig( PULSE_B_PIN, GPIO_INPUT );

    // Configure Pulse Pins Interrupt
    Chip_SCU_GPIOIntPinSel(PULSE_A_PININT_SWITCH_INDEX, PULSE_A_PIN_GPIO_PORT, PULSE_A_PIN_GPIO_PIN);
    Chip_SCU_GPIOIntPinSel(PULSE_B_PININT_SWITCH_INDEX, PULSE_B_PIN_GPIO_PORT, PULSE_B_PIN_GPIO_PIN);

    Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PULSE_A_PININT_SWITCH);
    Chip_PININT_SetPinModeEdge(LPC_GPIO_PIN_INT, PULSE_A_PININT_SWITCH);
    Chip_PININT_EnableIntLow(LPC_GPIO_PIN_INT, PULSE_A_PININT_SWITCH);

    Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PULSE_B_PININT_SWITCH);
    Chip_PININT_SetPinModeEdge(LPC_GPIO_PIN_INT, PULSE_B_PININT_SWITCH);
    Chip_PININT_EnableIntHigh(LPC_GPIO_PIN_INT, PULSE_B_PININT_SWITCH);


    /* Enable interrupt in the NVIC */
    pulseCounterEnableInt(true);
}


bool_t pulseCounterGet(pulseCount_t* result){
    pulseCounterEnableInt(false);
    pulseCount_t diff = 0;
    if(pulseACounter > pulseBCounter){
        diff = pulseACounter - pulseBCounter;
    } else {
        diff = pulseBCounter - pulseACounter;
    }
    bool_t success = diff <= pulseErrorThr;
    if(success){
        *result = (pulseCount_t)((pulseACounter + pulseBCounter)/2);
    }
    pulseACounter = 0;
    pulseBCounter = 0;
    pulseCounterEnableInt(true);
    return success;
}


void PULSE_A_PININT_IRQ_HANDLER(void)
{
    Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PULSE_A_PININT_SWITCH);
    pulseACounter++;
}


void PULSE_B_PININT_IRQ_HANDLER(void)
{
    Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PULSE_B_PININT_SWITCH);
    pulseBCounter++;
}

/* ------------------------------ End of file ------------------------------ */
