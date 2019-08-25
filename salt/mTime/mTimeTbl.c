/**
 *  \file       mTimeTble.c
 * 	\bried      mTime timers Table.
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
/* ----------------------------- Include files ----------------------------- */
#include <stdio.h>
#include "mTime.h"
#include "mTimeCfg.h"

#include "anIn.h"
#include "epoch.h"
#include "modpwr.h"
#include "pulseCounter.h"
#include "buzzer.h"
#include "relay.h"
#include "teloc.h"
#include "bsp.h"

/* ----------------------------- Local macros ------------------------------ */
/* ------------------------------- Constants ------------------------------- */
/* ---------------------------- Local data types --------------------------- */
/* ---------------------------- Global variables --------------------------- */
/* ---------------------------- Local variables ---------------------------- */
static void(* const actions_100[])( void ) =
{
#ifdef MODPWR_CTRL_ENABLE
	modPwr_ctrl, 
#endif
    //epoch_updateByStep,
    relayUpdate,
    //dIn_scan,
    NULL
};

static void(* const actions_1000[])( void ) =
{
    pulseCounterUpdate,
    buzzerUpdate,
    anInCaptureAndFilter,
#ifdef TELOC_TEST
    telocTestSend,
#endif
    NULL
};

static void(* const actions_10000[])( void ) =
{
	//anIn_update,
    NULL
};

const timerChain_t timerChain[] =
{
	{ MTIME_100_MS, actions_100 },
	{ MTIME_1_SEG, actions_1000 },
	{ MTIME_10_SEG, actions_10000 }
};

/* ----------------------- Local function prototypes ----------------------- */
/* ---------------------------- Local functions ---------------------------- */
/* ---------------------------- Global functions --------------------------- */
/* ------------------------------ End of file ------------------------------ */





