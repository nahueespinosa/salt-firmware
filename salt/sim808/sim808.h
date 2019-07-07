/**
 *  \file       sim808.h
 *  \brief      sim808 interface driver.
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
#ifndef __SIM808_H__
#define __SIM808_H__

/* ----------------------------- Include files ----------------------------- */
#include "sapi.h"
#include "rkh.h"

/* ---------------------- External C language linkage ---------------------- */
#ifdef __cplusplus
extern "C" {
#endif

/* --------------------------------- Macros -------------------------------- */
/* -------------------------------- Constants ------------------------------ */
/* ------------------------------- Data types ------------------------------ */

typedef enum{
    SIM_808_A, SIM_808_B, SIM_808_COUNT
} Sim808_t;

typedef enum{
    SIM_808_SLEEP, SIM_808_PWRKEY, SIM_808_VC, SIM_808_CONTROL_PIN_COUNT
} Sim808ControlPin_t;

typedef struct Sim808PinConfig_t{
    Sim808_t sim808;
    gpioMap_t sleepPin;
    gpioMap_t pwrkeyPin;
    gpioMap_t vcPin;
} Sim808PinConfig_t;

/* -------------------------- External variables --------------------------- */
/* -------------------------- Function prototypes -------------------------- */
void sim808Init(Sim808_t sim808);
void sim808SetControlPin( Sim808_t sim808,  Sim808ControlPin_t controlPin, bool_t data);
bool_t sim808GetControlPin( Sim808_t sim808,  Sim808ControlPin_t controlPin);

/* -------------------- External C language linkage end -------------------- */
#ifdef __cplusplus
}
#endif

/* ------------------------------ Module end ------------------------------- */
#endif

/* ------------------------------ End of file ------------------------------ */
