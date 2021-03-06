/**
 *  \file       buzzer.h
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
/* --------------------------------- Module -------------------------------- */
#ifndef __BUZZER_H__
#define __BUZZER_H__

/* ----------------------------- Include files ----------------------------- */
#include "sapi.h"

/* ---------------------- External C language linkage ---------------------- */
#ifdef __cplusplus
extern "C" {
#endif

/* --------------------------------- Macros -------------------------------- */
/* -------------------------------- Constants ------------------------------ */
/* ------------------------------- Data types ------------------------------ */
typedef enum BUZZER_MODE{
    BUZZER_MODE_DISABLE,
    BUZZER_MODE_BLINK,
    BUZZER_MODE_CONSTANT,
}BUZZER_MODE;
/* -------------------------- External variables --------------------------- */
/* -------------------------- Function prototypes -------------------------- */
void buzzerInit();
void buzzerInitBsp();
void buzzerSet(bool_t on);
bool_t buzzerGet();

void buzzerSetMode(BUZZER_MODE mode);
void buzzerUpdate();

/* -------------------- External C language linkage end -------------------- */
#ifdef __cplusplus
}
#endif

/* ------------------------------ Module end ------------------------------- */
#endif

/* ------------------------------ End of file ------------------------------ */
