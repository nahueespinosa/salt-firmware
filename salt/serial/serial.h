/**
 *  \file       serial.h
 *  \brief      serial interface driver.
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
#ifndef __SERIAL_H__
#define __SERIAL_H__

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
    UART_SIM_808_A, UART_SIM_808_B, UART_TELOC_1500, UART_DEBUG,
} serialMap_t;

typedef void (* serialIsrCb_t)(unsigned char c);

/* -------------------------- External variables --------------------------- */
/* -------------------------- Function prototypes -------------------------- */
void serialInit(serialMap_t serialMap);
void serialSetIntCb(serialMap_t serialMap, serialIsrCb_t cb );
void serialPutByte( serialMap_t uart, uint8_t byte );
void serialPutString( serialMap_t uart,  char *p );
void serialPutChars( serialMap_t uart,  unsigned char *p, ruint ndata );

/* -------------------- External C language linkage end -------------------- */
#ifdef __cplusplus
}
#endif

/* ------------------------------ Module end ------------------------------- */
#endif

/* ------------------------------ End of file ------------------------------ */
