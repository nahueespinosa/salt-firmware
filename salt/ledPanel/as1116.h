/**
 *  \file       as1116.h
 *  \brief      AS1116 driver.
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

/*
 * https://ar.mouser.com/datasheet/2/588/AS1116_DS000167_2-00-1511460.pdf
 *
 */

/* --------------------------------- Module -------------------------------- */
#ifndef __AS1116_H__
#define __AS1116_H__

/* ----------------------------- Include files ----------------------------- */
#include "rkh.h"
#include "sapi_spi.h"

/* ---------------------- External C language linkage ---------------------- */
#ifdef __cplusplus
extern "C" {
#endif

/* --------------------------------- Macros -------------------------------- */
/* -------------------------------- Constants ------------------------------ */

#define PANEL_REG_MASK				        0x1F
#define PANEL_READ_BIT				        (1<<6)
#define PANEL_NUM_DIGITS                    8

#define PANEL_SEGMENT_G_POS                 0
#define PANEL_SEGMENT_F_POS                 1
#define PANEL_SEGMENT_E_POS                 2
#define PANEL_SEGMENT_D_POS                 3
#define PANEL_SEGMENT_C_POS                 4
#define PANEL_SEGMENT_B_POS                 5
#define PANEL_SEGMENT_A_POS                 6
#define PANEL_SEGMENT_DP_POS                7

#define AS_REG_NOP					        0x00
#define PANEL_REG_DIGIT0				    0x01
#define PANEL_REG_DIGIT1				    0x02
#define PANEL_REG_DIGIT2				    0x03
#define PANEL_REG_DIGIT3				    0x04
#define PANEL_REG_DIGIT4				    0x05
#define PANEL_REG_DIGIT5				    0x06
#define PANEL_REG_DIGIT6				    0x07
#define PANEL_REG_DIGIT7				    0x08
#define PANEL_REG_DECODE_MODE			    0x09
#define PANEL_REG_GLOBAL_INTENSITY          0x0A
#define PANEL_REG_SCAN_LIMIT			    0x0B
#define PANEL_REG_SHUTDOWN				    0x0C
#define PANEL_REG_FEATURE				    0x0E
#define PANEL_REG_DISPLAY_TEST_MODE         0x0F
#define PANEL_REG_DIG01_INTENSITY		    0x10
#define PANEL_REG_DIG23_INTENSITY		    0x11
#define PANEL_REG_DIG45_INTENSITY		    0x12
#define PANEL_REG_DIG67_INTENSITY		    0x13
#define PANEL_REG_DIG0_DIAGNOSTIC		    0x14
#define PANEL_REG_DIG1_DIAGNOSTIC		    0x15
#define PANEL_REG_DIG2_DIAGNOSTIC		    0x16
#define PANEL_REG_DIG3_DIAGNOSTIC		    0x17
#define PANEL_REG_DIG4_DIAGNOSTIC		    0x18
#define PANEL_REG_DIG5_DIAGNOSTIC		    0x19
#define PANEL_REG_DIG6_DIAGNOSTIC		    0x1A
#define PANEL_REG_DIG7_DIAGNOSTIC		    0x1B

// feature register
#define PANEL_FEATURE_CLK_EN_bm				(1<<0)
#define PANEL_FEATURE_REG_RES_bm			(1<<1)
#define PANEL_FEATURE_DECODE_SEL_bm			(1<<2)
#define PANEL_FEATURE_BLINK_EN_bm			(1<<4)
#define PANEL_FEATURE_BLINK_FREQ_SEL_bm		(1<<5)
#define PANEL_FEATURE_SYNC_bm				(1<<6)
#define PANEL_FEATURE_BLINK_START_bm		(1<<7)

// shutdown register
#define PANEL_SHUTDOWN_OFF_MODE_RESET	    (0x00)
#define PANEL_SHUTDOWN_OFF_MODE			    (0x80)
#define PANEL_SHUTDOWN_NORMAL_MODE_RESET	(0x01)
#define PANEL_SHUTDOWN_NORMAL_MODE			(0x81)

static const rui8_t digitRegMap[] = {
        PANEL_REG_DIGIT0,
        PANEL_REG_DIGIT1,
        PANEL_REG_DIGIT2,
        PANEL_REG_DIGIT3,
        PANEL_REG_DIGIT4,
        PANEL_REG_DIGIT5,
        PANEL_REG_DIGIT6,
        PANEL_REG_DIGIT7,
};
/* ------------------------------- Data types ------------------------------ */
/* -------------------------- External variables --------------------------- */
/* -------------------------- Function prototypes -------------------------- */
void as1116Init(gpioMap_t sselPin);
void as1116SetDigit(rui8_t digit, rui8_t data);

/* -------------------- External C language linkage end -------------------- */
#ifdef __cplusplus
}
#endif

/* ------------------------------ Module end ------------------------------- */
#endif

/* ------------------------------ End of file ------------------------------ */
