/**
 *  \file       ledPanel.h
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
/* --------------------------------- Module -------------------------------- */
#ifndef __LEDPANEL_H__
#define __LEDPANEL_H__

/* ----------------------------- Include files ----------------------------- */
#include "rkht.h"
#include "sapi_spi.h"

/* ---------------------- External C language linkage ---------------------- */
#ifdef __cplusplus
extern "C" {
#endif

/* --------------------------------- Macros -------------------------------- */
/* -------------------------------- Constants ------------------------------ */
#define LED_R_MASK      (1 << 0)
#define LED_G_MASK      (1 << 1)
#define LED_B_MASK      (1 << 2)

#define NUM_DIGITS      4
#define NUM_COUNT       10

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
#define PANEL_REG_GLOBAL_INTENTSITY         0x0A
#define PANEL_REG_SCAN_LIMIT			    0x0B
#define PANEL_REG_SHUTDOWN				    0x0C
#define PANEL_REG_FEATURE				    0x0E
#define PANEL_REG_DISPLAY_TEST_MODE         0x0F
#define PANEL_REG_DIG01_INTENSITY		    0x10
#define PANEL_REG_DIG23_INTENSITY		    0x11
#define PANEL_REG_DIG45_INTENSITY		    0x12
#define PANEL_REG_DIG67_INTESITY		    0x13

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

#define PANEL_WRITE_BIT_bm				(1<<6)

static const rui8_t digitCodeMap[] = {
        //     GFEDCBA  Segments      7-segment map:
        0b00111111, // 0   "0"          AAA
        0b00000110, // 1   "1"         F   B
        0b01011011, // 2   "2"         F   B
        0b01001111, // 3   "3"          GGG
        0b01100110, // 4   "4"         E   C
        0b01101101, // 5   "5"         E   C
        0b01111101, // 6   "6"          DDD
        0b00000111, // 7   "7"
        0b01111111, // 8   "8"
        0b01101111, // 9   "9"
        0b01110111, // 65  'A'
        0b01111100, // 66  'b'
        0b00111001, // 67  'C'
        0b01011110, // 68  'd'
        0b01111001, // 69  'E'
        0b01110001, // 70  'F'
        0b00111101, // 71  'G'
        0b01110110, // 72  'H'
        0b00000110, // 73  'I'
        0b00001110, // 74  'J'
        0b01110110, // 75  'K'  Same as 'H'
        0b00111000, // 76  'L'
        0b00000000, // 77  'M'  NO DISPLAY
        0b01010100, // 78  'n'
        0b00111111, // 79  'O'
        0b01110011, // 80  'P'
        0b01100111, // 81  'q'
        0b01010000, // 82  'r'
        0b01101101, // 83  'S'
        0b01111000, // 84  't'
        0b00111110, // 85  'U'
        0b00111110, // 86  'V'  Same as 'U'
        0b00000000, // 87  'W'  NO DISPLAY
        0b01110110, // 88  'X'  Same as 'H'
        0b01101110, // 89  'y'
        0b01011011, // 90  'Z'  Same as '2'
        0b00000000, // 32  ' '  BLANK
        0b01000000, // 45  '-'  DASH
        0b10000000, // 46  '.'  PERIOD
        0b01100011, // 42 '*'  DEGREE ..
};

const rui8_t * const numeralCodes = digitCodeMap;
const rui8_t * const alphaCodes = digitCodeMap + 10;

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

typedef struct LedPanelCfg {
    rui8_t pointPosition; // digit with point if <4
    rui8_t digit0;
    rui8_t digit1;
    rui8_t digit2;
    rui8_t digit3;
    rui8_t led0;
    rui8_t led1;
    rui8_t led2;
    rui8_t led3;
    rui8_t led4;
    rui8_t led5;
} LedPanelCfg;


/* -------------------------- External variables --------------------------- */
/* -------------------------- Function prototypes -------------------------- */
void led_panel_init(void);
void led_panel_set_cfg(LedPanelCfg* cfg);

/* -------------------- External C language linkage end -------------------- */
#ifdef __cplusplus
}
#endif

/* ------------------------------ Module end ------------------------------- */
#endif

/* ------------------------------ End of file ------------------------------ */
