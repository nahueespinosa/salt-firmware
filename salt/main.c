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
#include "pulseCounter.h"
#include "serial.h"
#include "sim808.h"
#include "signals.h"
#include "bsp.h"

#include "conmgr.h"
#include "modmgr.h"
#include "mqttProt.h"
#include "publisher.h"

/* ----------------------------- Local macros ------------------------------ */
#define MQTTPROT_QSTO_SIZE  16
#define CONMGR_QSTO_SIZE    8
#define MODMGR_QSTO_SIZE    4

#define SIZEOF_EP0STO       16
#define SIZEOF_EP0_BLOCK    sizeof(RKH_EVT_T)
#define SIZEOF_EP1STO       128
#define SIZEOF_EP1_BLOCK    sizeof(ModCmd)
#define SIZEOF_EP2STO       512
#define SIZEOF_EP2_BLOCK    sizeof(ModMgrEvt)

/* ------------------------------- Constants ------------------------------- */

#define PULSE_COUNTER_THR 5

/* ---------------------------- Local data types --------------------------- */
/* ---------------------------- Global variables --------------------------- */
/* ---------------------------- Local variables ---------------------------- */
static RKH_EVT_T *MQTTProt_qsto[MQTTPROT_QSTO_SIZE];
static RKH_EVT_T *ConMgr_qsto[CONMGR_QSTO_SIZE];
static RKH_EVT_T *ModMgr_qsto[MODMGR_QSTO_SIZE];
static rui8_t evPool0Sto[SIZEOF_EP0STO],
        evPool1Sto[SIZEOF_EP1STO],
        evPool2Sto[SIZEOF_EP2STO];

static RKH_ROM_STATIC_EVENT(e_Open, evOpen);
static MQTTProtCfg mqttProtCfg;
static ModCmdRcvHandler simACmdParser;

/* ----------------------- Local function prototypes ----------------------- */
/* ---------------------------- Local functions ---------------------------- */


static void onSwitchCb(bool_t activated){
    //buzzerSet(activated);
}

static void simACb(unsigned char c){
    uartWriteByte(UART_USB,c);
    simACmdParser(c);
}

static void simBCb(unsigned char c){
    int i = 1;
}

static void telocCb(unsigned char c){
    int i = 2;
}

static void
saltConfig(void)
{
    /* Configuracion especifica SALT */

    /* Inicializacion SALT */

    bsp_init();
    relayInit();
    ledPanelInit();
    anInInit();
    buzzerInit();
    onSwitchInit(onSwitchCb);
    pulseCounterInit(PULSE_COUNTER_THR);

    sim808Init(SIM_808_A);
    serialSetIntCb(UART_SIM_808_A, simACb);
    sim808Init(SIM_808_B);
    serialSetIntCb(UART_SIM_808_B, simBCb);
    serialInit(UART_TELOC_1500);
    serialSetIntCb(UART_TELOC_1500, telocCb);

    /* Conexion de modulos */

    simACmdParser = ModCmd_init();



    //adcConfig(ADC_DISABLE);
    //dacConfig(DAC_DISABLE);
    //i2cConfig(I2C0, 100000);
    //uartConfig(UART_232, 19200);

    //gpioConfig( DI7, GPIO_INPUT ); // DI7 CIAA = ISP EDU-CIAA
    //gpioConfig( RS232_TXD, GPIO_INPUT );
    //gpioConfig( RS232_RXD, GPIO_INPUT );
    //gpioConfig( CAN_RD, GPIO_INPUT );
    //gpioConfig( CAN_TD, GPIO_INPUT );
    //gpioConfig( T_COL1, GPIO_INPUT );
    //gpioConfig( T_FIL0, GPIO_INPUT );
    //gpioConfig( T_FIL3, GPIO_INPUT );
    //gpioConfig( T_FIL2, GPIO_INPUT );
    //gpioConfig( T_COL0, GPIO_INPUT );
    //gpioConfig( T_FIL1, GPIO_INPUT );
    //gpioConfig( T_COL2, GPIO_INPUT );

    //gpioConfig( ENET_RXD0, GPIO_INPUT );
    //gpioConfig( LCDEN, GPIO_INPUT );
    //gpioConfig( GPIO0, GPIO_INPUT );
    //gpioConfig( GPIO2, GPIO_INPUT );
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
    //gpioConfig( ENET_CRS_DV, GPIO_INPUT );
    //gpioConfig( ENET_MDC, GPIO_INPUT );
    //gpioConfig( ENET_TXEN, GPIO_INPUT );
    //gpioConfig( ENET_RXD1, GPIO_INPUT );
}

static void
setupTraceFilters(void)
{
    RKH_FILTER_ON_GROUP(RKH_TRC_ALL_GROUPS);
    RKH_FILTER_ON_EVENT(RKH_TRC_ALL_EVENTS);
    //RKH_FILTER_OFF_EVENT(MODCMD_USR_TRACE);
    RKH_FILTER_OFF_EVENT(MODCMD_USR_TRACE_OUT);
    RKH_FILTER_OFF_EVENT(MODCMD_USR_TRACE_IN);
    //RKH_FILTER_OFF_GROUP_ALL_EVENTS(RKH_TG_USR);
    //RKH_FILTER_OFF_EVENT(RKH_TE_TMR_TOUT);
    RKH_FILTER_OFF_EVENT(RKH_TE_SM_STATE);
    //RKH_FILTER_OFF_EVENT(RKH_TE_SMA_FIFO);
    //RKH_FILTER_OFF_EVENT(RKH_TE_SMA_LIFO);
    //RKH_FILTER_OFF_EVENT(RKH_TE_SM_TS_STATE);
    //RKH_FILTER_OFF_EVENT(RKH_TE_SM_DCH);
    RKH_FILTER_OFF_SMA(modMgr);
    RKH_FILTER_OFF_SMA(conMgr);
    RKH_FILTER_OFF_SMA(mqttProt);
    RKH_FILTER_OFF_ALL_SIGNALS();
}

/* ---------------------------- Global functions --------------------------- */
void
saltCfg_clientId(char *pid)
{
    strcpy(mqttProtCfg.clientId, pid);
}

void
saltCfg_topic(char *t)
{
    sprintf(mqttProtCfg.topic, "/salt/%s", t);
}

int
main(int argc, char *argv[])
{

    saltConfig();

    rkh_fwk_init();

    setupTraceFilters();

    RKH_TRC_OPEN();

    rkh_dynEvt_init();
    rkh_fwk_registerEvtPool(evPool0Sto, SIZEOF_EP0STO, SIZEOF_EP0_BLOCK);
    rkh_fwk_registerEvtPool(evPool1Sto, SIZEOF_EP1STO, SIZEOF_EP1_BLOCK);
    rkh_fwk_registerEvtPool(evPool2Sto, SIZEOF_EP2STO, SIZEOF_EP2_BLOCK);

    mqttProtCfg.publishTime = MAX_PUBLISH_TIME;
    mqttProtCfg.syncTime = 4;
    mqttProtCfg.keepAlive = 400;
    mqttProtCfg.qos = 1;
    strcpy(mqttProtCfg.clientId, "");
    strcpy(mqttProtCfg.topic, "");
    MQTTProt_ctor(&mqttProtCfg, publishDimba);

    RKH_SMA_ACTIVATE(conMgr, ConMgr_qsto, CONMGR_QSTO_SIZE, 0, 0);
    RKH_SMA_ACTIVATE(modMgr, ModMgr_qsto, MODMGR_QSTO_SIZE, 0, 0);
    RKH_SMA_ACTIVATE(mqttProt, MQTTProt_qsto, MQTTPROT_QSTO_SIZE, 0, 0);

    RKH_SMA_POST_FIFO(conMgr, &e_Open, 0);

    rkh_fwk_enter();

    RKH_TRC_CLOSE();

    while(1){

        gpioWrite( LED1, ON );
        delay(500);

        gpioWrite( LED1, OFF );
        delay(500);

    }
    return 0;
}


/* ------------------------------ End of file ------------------------------ */