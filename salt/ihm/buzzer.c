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

/* ---------------------------- Local data types --------------------------- */
/* ---------------------------- Global variables --------------------------- */
/* ---------------------------- Local variables ---------------------------- */
BUZZER_MODE buzzerMode;
/* ----------------------- Local function prototypes ----------------------- */
/* ---------------------------- Local functions ---------------------------- */
/* ---------------------------- Global functions --------------------------- */

void buzzerInit(){
    buzzerInitBsp();
    buzzerSetMode(BUZZER_MODE_DISABLE);
}

void buzzerSetMode(BUZZER_MODE mode){
    buzzerMode = mode;
    buzzerUpdate();
}

void buzzerUpdate(){
    switch (buzzerMode){
        case BUZZER_MODE_DISABLE:
            buzzerSet(false);
            break;
        case BUZZER_MODE_BLINK:
            buzzerSet(!buzzerGet());
            break;
        case BUZZER_MODE_CONSTANT:
            buzzerSet(true);
            break;
        default:
            break;
    }
}

/* ------------------------------ End of file ------------------------------ */
