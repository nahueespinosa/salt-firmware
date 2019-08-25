/**
 *  \file       buzzer.c
 *  \brief      buzzer driver.
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
#include "buzzer.h"
#include "sapi.h"

/* ----------------------------- Local macros ------------------------------ */
/* ------------------------------- Constants ------------------------------- */

#define BUZZER_PIN          GPIO4

/* ---------------------------- Local data types --------------------------- */
/* ---------------------------- Global variables --------------------------- */
/* ---------------------------- Local variables ---------------------------- */
/* ----------------------- Local function prototypes ----------------------- */
/* ---------------------------- Local functions ---------------------------- */
/* ---------------------------- Global functions --------------------------- */

void buzzerInitBsp(){

    // Configure Buzzer Pin
    gpioWrite( BUZZER_PIN, false);
    gpioConfig( BUZZER_PIN, GPIO_OUTPUT );
}

void buzzerSet(bool_t on){
    //TODO volver a habilitar
    //gpioWrite( BUZZER_PIN, on);
}

bool_t buzzerGet(){
    return gpioRead(BUZZER_PIN);
}

/* ------------------------------ End of file ------------------------------ */
