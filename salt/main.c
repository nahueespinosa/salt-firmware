/**
 *  \file       main.c
 *  \brief      SALT application.
 */

/* -------------------------- Development history -------------------------- */
/*
 *  2019.06.21  IMD  v1.0.00  Initial version
 *
 */

/* -------------------------------- Authors -------------------------------- */
/*
 *  IMD  Ivan Mariano Di Vito divitoivan@gmail.com.com
 *
 */

/* --------------------------------- Notes --------------------------------- */
/* ----------------------------- Include files ----------------------------- */

#include <string.h>
#include <stdio.h>
#include "sapi.h"
#include "ledPanel.h"
#include "relay.h"
#include "anIn.h"
#include "buzzer.h"
#include "onSwitch.h"

/* ----------------------------- Local macros ------------------------------ */
/* ------------------------------- Constants ------------------------------- */
/* ---------------------------- Local data types --------------------------- */
/* ---------------------------- Global variables --------------------------- */
/* ---------------------------- Local variables ---------------------------- */
/* ----------------------- Local function prototypes ----------------------- */
/* ---------------------------- Local functions ---------------------------- */


static void onSwitchCb(bool_t activated){
    buzzerSet(activated);
}

static void
saltBoardConfig(void)
{

    /* Read clock settings and update SystemCoreClock variable */
    SystemCoreClockUpdate();

    Board_Init(); // From Board module (modules/lpc4337_m4/board)

    /* Inicializar el conteo de Ticks con resolución de 1ms, sin tickHook */
    tickConfig( 1, 0 );

    /* Inicializar GPIOs */
    gpioConfig( 0, GPIO_ENABLE );

    /* Configuración de pines de entrada para Teclas de la EDU-CIAA-NXP */
    gpioConfig( TEC1, GPIO_INPUT );
    gpioConfig( TEC2, GPIO_INPUT );
    gpioConfig( TEC3, GPIO_INPUT );
    gpioConfig( TEC4, GPIO_INPUT );

    /* Configuración de pines de salida para Leds de la EDU-CIAA-NXP */
    gpioConfig( LEDR, GPIO_OUTPUT );
    gpioConfig( LEDG, GPIO_OUTPUT );
    gpioConfig( LEDB, GPIO_OUTPUT );
    gpioConfig( LED1, GPIO_OUTPUT );
    gpioConfig( LED2, GPIO_OUTPUT );
    gpioConfig( LED3, GPIO_OUTPUT );

    /* Configuracion especifica SALT */

    //adcConfig(ADC_DISABLE);
    //dacConfig(DAC_DISABLE);
    //i2cConfig(I2C0, 100000);
    //uartConfig(UART_232, 19200);

    //gpioConfig( DI7, GPIO_INPUT ); // DI7 CIAA = ISP EDU-CIAA
    gpioConfig( RS232_TXD, GPIO_INPUT );
    gpioConfig( RS232_RXD, GPIO_INPUT );
    gpioConfig( CAN_RD, GPIO_INPUT );
    gpioConfig( CAN_TD, GPIO_INPUT );
    //gpioConfig( T_COL1, GPIO_INPUT );
    //gpioConfig( T_FIL0, GPIO_INPUT );
    //gpioConfig( T_FIL3, GPIO_INPUT );
    //gpioConfig( T_FIL2, GPIO_INPUT );
    //gpioConfig( T_COL0, GPIO_INPUT );
    //gpioConfig( T_FIL1, GPIO_INPUT );
    //gpioConfig( T_COL2, GPIO_INPUT );

    gpioConfig( ENET_RXD0, GPIO_INPUT );
    gpioConfig( LCDEN, GPIO_INPUT );
    gpioConfig( GPIO0, GPIO_INPUT );
    gpioConfig( GPIO2, GPIO_INPUT );
    //gpioConfig( GPIO4, GPIO_INPUT );
    //gpioConfig( GPIO6, GPIO_INPUT );
    //gpioConfig( GPIO8, GPIO_INPUT );
    //gpioConfig( GPIO7, GPIO_INPUT );
    //gpioConfig( GPIO5, GPIO_INPUT );
    //gpioConfig( GPIO3, GPIO_INPUT );
    //gpioConfig( GPIO1, GPIO_INPUT );
    //gpioConfig( LCD1, GPIO_INPUT );
    //gpioConfig( LCD2, GPIO_INPUT );
    //gpioConfig( LCD3, GPIO_INPUT );
    //gpioConfig( LCDRS, GPIO_INPUT );
    //gpioConfig( LCD4, GPIO_INPUT );
    //gpioConfig( ENET_TXD1, GPIO_INPUT );
    //gpioConfig( ENET_TXD0, GPIO_INPUT );
    //gpioConfig( ENET_MDIO, GPIO_INPUT );
    gpioConfig( ENET_CRS_DV, GPIO_INPUT );
    gpioConfig( ENET_MDC, GPIO_INPUT );
    gpioConfig( ENET_TXEN, GPIO_INPUT );
    gpioConfig( ENET_RXD1, GPIO_INPUT );

    relayInit();
    ledPanelInit();
    anInInit();
    buzzerInit();
    onSwitchInit();

    onSwitchSetIntCb(onSwitchCb);
}

/* ---------------------------- Global functions --------------------------- */

int
main(int argc, char *argv[])
{

    saltBoardConfig();

    while(1){

        gpioWrite( LED1, ON );
        delay(500);

        gpioWrite( LED1, OFF );
        delay(500);
    }
    return 0;
}


/* ------------------------------ End of file ------------------------------ */