/**
 *  \file       as1116.c
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
#include "as1116.h"
#include "sapi.h"
#include "sapi_spi.h"

/* ----------------------------- Local macros ------------------------------ */

#define R2(n)     n,     n + 2*64,     n + 1*64,     n + 3*64
#define R4(n) R2(n), R2(n + 2*16), R2(n + 1*16), R2(n + 3*16)
#define R6(n) R4(n), R4(n + 2*4 ), R4(n + 1*4 ), R4(n + 3*4 )

/* ------------------------------- Constants ------------------------------- */

static const unsigned char BitReverseTable256[256] =
        {
                R6(0), R6(2), R6(1), R6(3)
        };

/* ---------------------------- Local data types --------------------------- */
/* ---------------------------- Global variables --------------------------- */
/* ---------------------------- Local variables ---------------------------- */

rui8_t as1116Buffer[2];
gpioMap_t as1116SselPin = 0;
rbool_t initilized = false;

/* ----------------------- Local function prototypes ----------------------- */

void as1116WriteReg(rui8_t reg, rui8_t data);
rui8_t as1116ReadReg(rui8_t reg);

/* ---------------------------- Local functions ---------------------------- */

void as1116WriteReg(rui8_t reg, rui8_t data){
    if( !initilized ){
        return;
    }

    as1116Buffer[0] = BitReverseTable256[data];
    as1116Buffer[1] = BitReverseTable256[(reg & PANEL_REG_MASK)];

    gpioWrite( as1116SselPin, false);
    spiWrite( SPI0, as1116Buffer, 2);
    while ( Chip_SSP_GetStatus(LPC_SSP1, SSP_STAT_BSY) ){ }
    gpioWrite( as1116SselPin, true);
}

rui8_t as1116ReadReg(rui8_t reg){
    if( !initilized ){
        return 0;
    }

    as1116Buffer[0] = BitReverseTable256[(reg & PANEL_REG_MASK) | PANEL_READ_BIT] ;
    as1116Buffer[1] = 0x00;

    gpioWrite( as1116SselPin, false);
    spiWrite( SPI0, as1116Buffer, 1);
    while ( Chip_SSP_GetStatus(LPC_SSP1, SSP_STAT_BSY) ){ }
    gpioWrite( as1116SselPin, true);
    spiRead( SPI0, as1116Buffer+1, 1);
    while ( Chip_SSP_GetStatus(LPC_SSP1, SSP_STAT_BSY) ){ }

    return as1116Buffer[1];
}

/* ---------------------------- Global functions --------------------------- */

void as1116Init(gpioMap_t sselPin){

    as1116SselPin = sselPin;
    initilized = true;

    // Configure SSP SSP1 pins
    Chip_SCU_PinMuxSet(0xf, 4, (SCU_MODE_PULLUP | SCU_MODE_FUNC0)); // CLK0
    Chip_SCU_PinMuxSet(0x1, 3, (SCU_MODE_PULLUP | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_FUNC5)); // MISO1
    Chip_SCU_PinMuxSet(0x1, 4, (SCU_MODE_PULLUP | SCU_MODE_FUNC5)); // MOSI1

    // Configure SSEL soft Pin
    gpioConfig( as1116SselPin, GPIO_OUTPUT );
    gpioWrite( as1116SselPin, true);

    // Initialize SSP Peripheral
    Chip_SSP_Init( LPC_SSP1 );
    Chip_SSP_SetFormat(LPC_SSP1, SSP_BITS_8, SSP_FRAMEFORMAT_SPI, SSP_CLOCK_CPHA1_CPOL0);
    Chip_SSP_Enable( LPC_SSP1 );

    //Init AS1116
    as1116WriteReg(PANEL_REG_SHUTDOWN,PANEL_SHUTDOWN_NORMAL_MODE_RESET);
    as1116WriteReg(PANEL_REG_DECODE_MODE,0x00);
    as1116WriteReg(PANEL_REG_GLOBAL_INTENSITY,0x03);
    as1116WriteReg(PANEL_REG_SCAN_LIMIT,0x06);
    as1116WriteReg(PANEL_REG_FEATURE, 0x00);

}

void as1116SetDigit(rui8_t digit, rui8_t data){
    if(digit >= PANEL_NUM_DIGITS){
        return;
    }
    as1116WriteReg(digitRegMap[digit],data);
}

/* ------------------------------ End of file ------------------------------ */
