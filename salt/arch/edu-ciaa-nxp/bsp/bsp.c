/**
 *  \file       bsp.c
 *  \brief      BSP for EDU-CIAA-NXP Prototype
 *
 *  \ingroup    bsp
 */

/* -------------------------- Development history -------------------------- */
/*
 *  2017.04.14  DaBa  v0.0.01  Initial version
 */

/* -------------------------------- Authors -------------------------------- */
/*
 *  DaBa  Dario Baliña       db@vortexmakes.com
 *  LeFr  Leandro Francucci  lf@vortexmakes.com
 */

/* --------------------------------- Notes --------------------------------- */
/* ----------------------------- Include files ----------------------------- */
#include <stdio.h>
#include "rkh.h"
#include "sapi.h"
#include "bsp.h"

#include "signals.h"
#include "modpwr.h"
#include "modmgr.h"
#include "modcmd.h"
#include "conmgr.h"
#include "mTime.h"
#include "anIn.h"
#include "epoch.h"
#include "rtime.h"

RKH_THIS_MODULE

/* ----------------------------- Local macros ------------------------------ */
#define RegStatus(b)        gpioWrite(LED2, b)
#define NetStatus(b)        gpioWrite(LED3, b)

/* ------------------------------- Constants ------------------------------- */
/* ---------------------------- Local data types --------------------------- */
/* ---------------------------- Global variables --------------------------- */
/* ---------------------------- Local variables ---------------------------- */
static RKH_TS_T tstamp;

/* ----------------------- Local function prototypes ----------------------- */
/* ---------------------------- Local functions ---------------------------- */
/* ---------------------------- Global functions --------------------------- */
void
bsp_init()
{
    /* Read clock settings and update SystemCoreClock variable */
    SystemCoreClockUpdate();

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

    bsp_modStatus(0, DisconnectedSt);
    bsp_modStatus(1, DisconnectedSt);
    bsp_regStatus(0, UnregisteredSt);
    bsp_regStatus(1, UnregisteredSt);
    bsp_netStatus(0, DisconnectedSt);
    bsp_netStatus(1, DisconnectedSt);

    modPwr_init();
    rtime_init();
}

void
bsp_timeTick(void)
{
    ++tstamp;
    
    modPwr_ctrl(); //TODO me parece que esto esta mal aca, se repite en mTime
    mTime_tick();
}

RKH_TS_T
rkh_trc_getts(void)
{
    return tstamp;
}

void
bsp_regStatus(int ch, Status_t status)
{
    switch(ch){
        case 0:
            gpioWrite(LED2, status);
            break;
        case 1:
            gpioWrite(LEDG, status);
            break;
        default:
            break;
    }
}

void 
bsp_netStatus(int ch, Status_t status)
{
    switch(ch){
        case 0:
            gpioWrite(LED3, status);
            break;
        case 1:
            gpioWrite(LEDB, status);
            break;
        default:
            break;
    }
}

void bsp_modStatus(int ch, Status_t status){
    switch(ch){
        case 0:
            gpioWrite(LED1, status);
            break;
        case 1:
            gpioWrite(LEDR, status);
            break;
        default:
            break;
    }
}

void 
bsp_modStatusToggle(int ch)
{
    switch(ch){
        case 0:
            gpioToggle(LED1);
            break;
        case 1:
            gpioToggle(LEDR);
            break;
        default:
            break;
    }
}


/* ------------------------------ File footer ------------------------------ */
