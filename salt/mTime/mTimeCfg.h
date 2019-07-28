/**
 *  \file       mTimeCfg.h
 * 	\bried      Main timer definitions.
 */

/* -------------------------- Development history -------------------------- */
/*
 *  2018.05.17  DaBa  v1.0.00  Initial version
 */

/* -------------------------------- Authors -------------------------------- */
/*
 *  DaBa  Dario Baliña       db@vortexmakes.com
 */

/* --------------------------------- Notes --------------------------------- */
/* --------------------------------- Module -------------------------------- */
#ifndef __MTIMECFG_H__
#define __MTIMECFG_H__

/* ----------------------------- Include files ----------------------------- */
#include "rkh.h"

/* ---------------------- External C language linkage ---------------------- */
#ifdef __cplusplus
extern "C" {
#endif

/* --------------------------------- Macros -------------------------------- */
/* -------------------------------- Constants ------------------------------ */
/*
 * 	Here define the time of interrupt in us
 */
#define MTIME_TIME_TICK			RKH_TICK_RATE_MS   /* Expressed in msec */

/*
 * 	Number of slots in table 'timerChain'declared in 'mTimeTbl.h' and
 *  user defined in 'mTimeTbl.c'.
 */
#define NUM_TIMER_DIVISORS		3

/*
 * Scaled time slots constants
 */
#define MTIME_100_MS			(100/MTIME_TIME_TICK)
#define MTIME_1_SEG             (1000/MTIME_TIME_TICK)
#define MTIME_10_SEG            (10000/MTIME_TIME_TICK)

#define MTIME_MODPWR_BASE                   MTIME_100_MS
#define MTIME_EPOCH_UPD_PERIOD			    MTIME_100_MS
#define MTIME_ANIN_READANDFILTER_PERIOD     MTIME_1_SEG
#define MTIME_ANSAMPLE_PUT_PERIOD           MTIME_10_SEG

/* ------------------------------- Data types ------------------------------ */
/* -------------------------- External variables --------------------------- */
/* -------------------------- Function prototypes -------------------------- */
/* -------------------- External C language linkage end -------------------- */
#ifdef __cplusplus
}
#endif

/* ------------------------------ Module end ------------------------------- */
#endif

/* ------------------------------ End of file ------------------------------ */
/*
 * 	mTimeCfg.h
 */

