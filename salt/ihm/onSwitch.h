/**
 *  \file       onSwitch.h
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
/* --------------------------------- Module -------------------------------- */
#ifndef __ON_SWITCH_H__
#define __ON_SWITCH_H__

/* ----------------------------- Include files ----------------------------- */
#include "sapi.h"

/* ---------------------- External C language linkage ---------------------- */
#ifdef __cplusplus
extern "C" {
#endif

/* --------------------------------- Macros -------------------------------- */
/* -------------------------------- Constants ------------------------------ */
/* ------------------------------- Data types ------------------------------ */
typedef void (* onSwitchCb_t)(bool_t activated);

/* -------------------------- External variables --------------------------- */
/* -------------------------- Function prototypes -------------------------- */
void onSwitchInit(onSwitchCb_t cb);
bool_t onSwitchGet();
void onSwitchSetIntCb( onSwitchCb_t cb );

/* -------------------- External C language linkage end -------------------- */
#ifdef __cplusplus
}
#endif

/* ------------------------------ Module end ------------------------------- */
#endif

/* ------------------------------ End of file ------------------------------ */
