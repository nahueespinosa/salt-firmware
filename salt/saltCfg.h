/**
 *  \file       salt.h
 *  \brief      Specification for configure Dimba.
 */

/* -------------------------- Development history -------------------------- */
/*
 *  2018.05.17  DaBa  v1.0.00  Initial version
 */

/* -------------------------------- Authors -------------------------------- */
/*
 *  DaBa  Dario Bali�a       db@vortexmakes.com
 */

/* --------------------------------- Notes --------------------------------- */
/* --------------------------------- Module -------------------------------- */
#ifndef __SALTCFG_H__
#define __SALTCFG_H__

/* ----------------------------- Include files ----------------------------- */
/* ---------------------- External C language linkage ---------------------- */
#ifdef __cplusplus
extern "C" {
#endif

/* --------------------------------- Macros -------------------------------- */
/* -------------------------------- Constants ------------------------------ */
enum RKH_SMA_PRIORITY{
    MOD_MGR_A_PRIORITY,
    MOD_MGR_B_PRIORITY,
    CON_MGR_A_PRIORITY,
    CON_MGR_B_PRIORITY,
    MQTT_PRIORITY,
    MQTT_SYNC_PRIORITY,
    RKH_SMA_COUNT
};
/* ------------------------------- Data types ------------------------------ */
/* -------------------------- External variables --------------------------- */
/* -------------------------- Function prototypes -------------------------- */
void saltCfg_clientId(char *pid);
void saltCfg_topic(char *topic);

/* -------------------- External C language linkage end -------------------- */
#ifdef __cplusplus
}
#endif

/* ------------------------------ Module end ------------------------------- */
#endif

/* ------------------------------ End of file ------------------------------ */
