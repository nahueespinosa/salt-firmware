/**
 *  \file       gps.h
 *  \brief      Specification of gps data.
 */

/* -------------------------- Development history -------------------------- */
/*
 *  2018.04.08  IMD  v1.0.00  Initial version
 */

/* -------------------------------- Authors -------------------------------- */
/*
 *  IMD  Ivan Mariano Di Vito divitoivan@gmail.com
 */

/* --------------------------------- Notes --------------------------------- */
/* --------------------------------- Module -------------------------------- */
#ifndef __GPS_H__
#define __GPS_H__

/* ----------------------------- Include files ----------------------------- */
#include "epoch.h"
#include "sapi.h"
/* ---------------------- External C language linkage ---------------------- */
#ifdef __cplusplus
extern "C" {
#endif

/* --------------------------------- Macros -------------------------------- */
/* -------------------------------- Constants ------------------------------ */
/* ------------------------------- Data types ------------------------------ */

typedef struct GpsData GpsData;
struct GpsData
{
    Time            time;
    bool_t          valid;
    unsigned char   latitude_deg;
    float           latitude_min;
    bool_t          latitude_north;
    unsigned char   longitude_deg;
    float           longitude_min;
    bool_t          longitude_east;
    float           speed;
    float           course;
};

/* -------------------------- External variables --------------------------- */
/* -------------------------- Function prototypes -------------------------- */
/* -------------------- External C language linkage end -------------------- */
#ifdef __cplusplus
}
#endif

/* ------------------------------ Module end ------------------------------- */
#endif

/* ------------------------------ End of file ------------------------------ */
