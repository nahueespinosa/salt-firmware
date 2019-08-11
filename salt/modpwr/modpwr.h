/**
 *  \file       modpwr.h
 *  \brief      Specification of modpwr abstraction.
 */

/* -------------------------- Development history -------------------------- */
/*
 *  2018.06.05  DaBa  v1.0.00  Initial version
 */

/* -------------------------------- Authors -------------------------------- */
/*
 *  DaBa  Dario Bali�a       db@vortexmakes.com
 */

/* --------------------------------- Notes --------------------------------- */
/* --------------------------------- Module -------------------------------- */
#ifndef __MODPWR_H__
#define __MODPWR_H__

/* ----------------------------- Include files ----------------------------- */
/* ---------------------- External C language linkage ---------------------- */
#ifdef __cplusplus
extern "C" {
#endif

/* --------------------------------- Macros -------------------------------- */
/* -------------------------------- Constants ------------------------------ */
/* ------------------------------- Data types ------------------------------ */
typedef enum ModPwrIndex
{
    ModPwrA,
    ModPwrB,
    ModPwrCount,
}ModPwrIndex;
/* -------------------------- External variables --------------------------- */
/* -------------------------- Function prototypes -------------------------- */
#ifdef MODPWR_CTRL_ENABLE

void modPwr_init(void);
void modPwr_ctrl(void);
void modPwr_on(ModPwrIndex index);
void modPwr_off(ModPwrIndex index);

#else

#define modPwr_init();
#define modPwr_ctrl();
#define modPwr_on();
#define modPwr_off();

#endif

/* -------------------- External C language linkage end -------------------- */
#ifdef __cplusplus
}
#endif

/* ------------------------------ Module end ------------------------------- */
#endif

/* ------------------------------ End of file ------------------------------ */
