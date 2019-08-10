/**
 *  \file       bsp.h
 *  \brief      BSP for 80x86 OS win32
 *
 *  \ingroup    bsp
 */

/* -------------------------- Development history -------------------------- */
/*
 *  2018.05.23  DaBa  v0.0.01  Initial version
 *  2019.06.17  IMD  v0.0.02  SALT changes
 *
 */

/* -------------------------------- Authors -------------------------------- */
/*
 *  LeFr  Leandro Francucci  lf@vortexmakes.com
 *  DaBa  Dario Bali�a       dariosb@gmail.com
 *  IMD  Ivan Mariano Di Vito divitoivan@gmail.com.com
 */

/* --------------------------------- Module -------------------------------- */
#ifndef __BSP_H__
#define __BSP_H__

/* ----------------------------- Include files ----------------------------- */
#include "rkh.h"


/* ---------------------- External C language linkage ---------------------- */


#ifdef __cplusplus
extern "C" {
#endif

/* --------------------------------- Macros -------------------------------- */
/* -------------------------------- Constants ------------------------------ */

#define DEBUG_SERIAL

#define SEND_TRACE
//#define DEBUG_SERIAL_PASS

/*
 * User trace events id�s
 */
enum
{
    USR_TRACE = RKH_TE_USER,
    USR_TRACE_OUT,
    USR_TRACE_EVT,
    USR_TRACE_IN,
    USR_TRACE_SSP,
    USR_TRACE_MQTT,
};

/*
 * Status Led�s 
 */
typedef enum
{
    DisconnectedSt, 
    UnregisteredSt = DisconnectedSt,

    ConnectedSt,
    RegisteredSt = ConnectedSt,
} Status_t;

/*
 * Status Led�s 
 */

/* ------------------------------- Data types ------------------------------ */
/* -------------------------- External variables --------------------------- */
/* -------------------------- Function prototypes -------------------------- */
void bsp_init();

void bsp_keyParser(int c);
void bsp_timeTick(void);

void bsp_serial_open(int ch);
void bsp_serial_close(int ch);
void bsp_serial_puts(int ch, char *p);
void bsp_serial_putnchar(int ch, unsigned char *p, ruint ndata);

void bsp_regStatus(Status_t status);
void bsp_netStatus(Status_t status);
void bsp_modStatus(int ch, Status_t status);
void bsp_modStatusToggle(int ch);

/* -------------------- External C language linkage end -------------------- */
#ifdef __cplusplus
}
#endif

/* ------------------------------ Module end ------------------------------- */
#endif

/* ------------------------------ File footer ------------------------------ */
