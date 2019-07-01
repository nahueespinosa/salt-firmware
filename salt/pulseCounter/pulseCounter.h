/**
 *  \file       pulseCounter.h
 *  \brief      Tachometer pulse counter driver.
 */

/* -------------------------- Development history -------------------------- */
/*
 *  2019.06.17  IMD  v1.0.00  SALT changes
 *
 */

/* -------------------------------- Authors -------------------------------- */
/*
 *  IMD  Ivan Mariano Di Vito divitoivan@gmail.com.com
 */

/* --------------------------------- Notes --------------------------------- */
/* --------------------------------- Module -------------------------------- */
#ifndef __PULSE_COUNTER_H__
#define __PULSE_COUNTER_H__

/* ----------------------------- Include files ----------------------------- */
#include "rkh.h"
#include "sapi.h"
/* ---------------------- External C language linkage ---------------------- */
#ifdef __cplusplus
extern "C" {
#endif

/* --------------------------------- Macros -------------------------------- */
/* -------------------------------- Constants ------------------------------ */
/* ------------------------------- Data types ------------------------------ */
typedef rui32_t pulseCount_t;
/* -------------------------- External variables --------------------------- */
/* -------------------------- Function prototypes -------------------------- */
void pulseCounterInit(pulseCount_t errorThr);
bool_t pulseCounterGet(pulseCount_t* result);

/* -------------------- External C language linkage end -------------------- */
#ifdef __cplusplus
}
#endif

/* ------------------------------ Module end ------------------------------- */
#endif

/* ------------------------------ End of file ------------------------------ */
