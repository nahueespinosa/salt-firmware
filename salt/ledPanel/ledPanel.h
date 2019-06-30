/**
 *  \file       ledPanel.h
 *  \brief      Led panel controller driver.
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
 * https://github.com/DeanIsMe/SevSeg
 *
 */

/* --------------------------------- Module -------------------------------- */
#ifndef __LEDPANEL_H__
#define __LEDPANEL_H__

/* ----------------------------- Include files ----------------------------- */
#include "rkh.h"
#include "sapi_spi.h"

/* ---------------------- External C language linkage ---------------------- */
#ifdef __cplusplus
extern "C" {
#endif

/* --------------------------------- Macros -------------------------------- */
/* -------------------------------- Constants ------------------------------ */
#define LED_R_POS       0
#define LED_G_POS       2
#define LED_B_POS       1
#define LED_R_MASK      (1 << LED_R_POS)
#define LED_G_MASK      (1 << LED_G_POS)
#define LED_B_MASK      (1 << LED_B_POS)

#define NUM_DIGITS      4
#define NUM_COUNT       10

#define DIGIT_RED_POS   0
#define DIGIT_BLUE_POS  1
#define DIGIT_GREEN_POS 2
#define DIGIT_0_POS     3
#define DIGIT_1_POS     4
#define DIGIT_2_POS     5
#define DIGIT_3_POS     6

#define LED_ON_SEGMENT_POS          PANEL_SEGMENT_A_POS
#define LED_CT_SEGMENT_POS          PANEL_SEGMENT_E_POS
#define LED_FE_SEGMENT_POS          PANEL_SEGMENT_C_POS
#define LED_GPS_SEGMENT_POS         PANEL_SEGMENT_D_POS
#define LED_REMOTE_OP_SEGMENT_POS      PANEL_SEGMENT_DP_POS
#define LED_ISOLATED_SEGMENT_POS    PANEL_SEGMENT_F_POS

static const rui8_t digitCodeMap[] = {
        //     DpABCDEFG  Segments      7-segment map:
        0b01111110, // 0   "0"          AAA
        0b00110000, // 1   "1"         F   B
        0b01101101, // 2   "2"         F   B
        0b01111001, // 3   "3"          GGG
        0b00110011, // 4   "4"         E   C
        0b01011011, // 5   "5"         E   C
        0b01011111, // 6   "6"          DDD
        0b01110000, // 7   "7"
        0b01111111, // 8   "8"
        0b01111011, // 9   "9"
        0b01110111, // 10  'A'
        0b00011111, // 11  'b'
        0b01001110, // 12  'C'
        0b00111101, // 13  'd'
        0b01001111, // 14  'E'
        0b01000111, // 15  'F'
        0b01011110, // 16  'G'
        0b00110111, // 17  'H'
        0b00110000, // 18  'I'
        0b00111000, // 19  'J'
        0b00110111, // 20  'K'  Same as 'H'
        0b00001110, // 21  'L'
        0b00000000, // 22  'M'  NO DISPLAY
        0b00010101, // 23  'n'
        0b01111110, // 24  'O'
        0b01100111, // 25  'P'
        0b01110011, // 26  'q'
        0b00000101, // 27  'r'
        0b01011011, // 28  'S'
        0b00001111, // 29  't'
        0b00111110, // 30  'U'
        0b00111110, // 31  'V'  Same as 'U'
        0b00000000, // 32  'W'  NO DISPLAY
        0b00110111, // 33  'X'  Same as 'H'
        0b00111011, // 34  'y'
        0b01101101, // 35  'Z'  Same as '2'
        0b00000000, // 36  ' '  BLANK
        0b00000001, // 37  '-'  DASH
        0b10000000, // 38  '.'  PERIOD
        0b01100011, // 39  '*'  DEGREE ..
};

extern const rui8_t * const numeralCodes;
extern const rui8_t * const alphaCodes;

#define PERIOD_ALPHA_POS 38

/* ------------------------------- Data types ------------------------------ */

typedef enum{
    BLACK   = 0x00,
    WHITE   = LED_R_MASK | LED_G_MASK | LED_B_MASK,
    RED     = LED_R_MASK,
    GREEN   = LED_G_MASK,
    BLUE    = LED_B_MASK,
    YELLOW  = LED_R_MASK | LED_G_MASK,
    CYAN    = LED_G_MASK | LED_B_MASK,
    MAGENTA = LED_R_MASK | LED_B_MASK,
} LedRgbCfg_t;

typedef struct LedPanelCfg {
    rui8_t pointPosition; // digit with point if <4
    rui8_t digit0;
    rui8_t digit1;
    rui8_t digit2;
    rui8_t digit3;
    LedRgbCfg_t ledOn;
    LedRgbCfg_t ledCt;
    LedRgbCfg_t ledFe;
    LedRgbCfg_t ledGps;
    LedRgbCfg_t ledRemoteOp;
    LedRgbCfg_t ledIsolated;
} LedPanelCfg;


/* -------------------------- External variables --------------------------- */
/* -------------------------- Function prototypes -------------------------- */
void ledPanelInit(void);
void ledPanelSetCfg(LedPanelCfg* cfg);
void ledPanelTestSalt(void);

/* -------------------- External C language linkage end -------------------- */
#ifdef __cplusplus
}
#endif

/* ------------------------------ Module end ------------------------------- */
#endif

/* ------------------------------ End of file ------------------------------ */
