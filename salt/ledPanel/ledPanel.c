/**
 *  \file       ledPanel.c
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
/* ----------------------------- Include files ----------------------------- */
#include "ledPanel.h"

/* ----------------------------- Local macros ------------------------------ */
/* ------------------------------- Constants ------------------------------- */
/* ---------------------------- Local data types --------------------------- */
/* ---------------------------- Global variables --------------------------- */
/* ---------------------------- Local variables ---------------------------- */
/* ----------------------- Local function prototypes ----------------------- */
/* ---------------------------- Local functions ---------------------------- */
/* ---------------------------- Global functions --------------------------- */

void led_panel_init(void){

    spiConfig(SPI0);

}


void led_panel_set_cfg(LedPanelCfg* cfg){
    rui8_t digits[NUM_DIGITS];
    rui8_t buffer[2];

    if(cfg->digit0 < NUM_COUNT){
        digits[0] = numeralCodes[cfg->digit0];
    }
    if(cfg->digit1 < NUM_COUNT){
        digits[1] = numeralCodes[cfg->digit1];
    }
    if(cfg->digit2 < NUM_COUNT){
        digits[2] = numeralCodes[cfg->digit2];
    }
    if(cfg->digit3 < NUM_COUNT){
        digits[3] = numeralCodes[cfg->digit3];
    }

    for (uint8_t i = 0; i < NUM_DIGITS; i++)
    {
        buffer[0] = digits[i];
        buffer[1] = digitRegMap[i] | PANEL_WRITE_BIT_bm;
        spiWrite(SPI0,buffer, sizeof(buffer));
    }

}

/* ------------------------------ End of file ------------------------------ */
