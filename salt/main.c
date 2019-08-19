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
#include "modpwr.h"
#include "teloc.h"
#include "saltCmd.h"

#include "conmgr.h"
#include "modmgr.h"
#include "mqttProt.h"
#include "publisher.h"
#include "logic.h"

/* ----------------------------- Local macros ------------------------------ */
#define LOGIC_QSTO_SIZE  16
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

#define PULSE_COUNTER_THR                   5
#define PULSE_COUNTER_FACTOR                9.0432 // m/s_km/h(3.6) * pi * d_rueda(0.8m) / pulsos_revolucion(1)

#define PWR_INPUT_SAMPLE_MIN                60 // 60V
#define PWR_INPUT_SAMPLE_MAX                120 // 110V

/* ---------------------------- Local data types --------------------------- */

/* ---------------------------- Global variables --------------------------- */
/* ---------------------------- Local variables ---------------------------- */
static RKH_EVT_T *Logic_qsto[LOGIC_QSTO_SIZE];
static RKH_EVT_T *MQTTProt_qsto[MQTTPROT_QSTO_SIZE];
static RKH_EVT_T *ConMgr_qsto[CONMGR_QSTO_SIZE];
static RKH_EVT_T *ModMgr_qsto[MODMGR_QSTO_SIZE];
static rui8_t evPool0Sto[SIZEOF_EP0STO],
        evPool1Sto[SIZEOF_EP1STO],
        evPool2Sto[SIZEOF_EP2STO];

static RKH_ROM_STATIC_EVENT(e_Open, evOpen);
static RKH_ROM_STATIC_EVENT(e_SaltEnable, evSaltEnable);
static RKH_ROM_STATIC_EVENT(e_SaltDisable, evSaltDisable);
static CmdEvt e_saltCmd;
static MQTTProtCfg mqttProtCfg;
static LogicCfg logicCfg;
static ModCmdRcvHandler simACmdParser = NULL;
static rbool_t initEnd = false;
static rbool_t pwrCorrect = true;

/* ----------------------- Local function prototypes ----------------------- */
/* ---------------------------- Local functions ---------------------------- */

static void onAnInCb(){
    if(!initEnd){
        return;
    }

    sample_t sample = anInGetSample(anIn0);
    bool_t aux = sample > PWR_INPUT_SAMPLE_MIN && sample < PWR_INPUT_SAMPLE_MAX;
    if(aux != pwrCorrect){
        pwrCorrect = aux;
        if(pwrCorrect && onSwitchGet()){
            RKH_SMA_POST_FIFO(logic, &e_SaltEnable, 0);
        } else {
            RKH_SMA_POST_FIFO(logic, &e_SaltDisable, 0);
        }
    }
}

static void onSwitchCb(bool_t activated){
    if(!initEnd){
        return;
    }
    if(!pwrCorrect){
        return;
    }

    if(activated){
        RKH_SMA_POST_FIFO(logic, &e_SaltEnable, 0);
    } else {
        RKH_SMA_POST_FIFO(logic, &e_SaltDisable, 0);
    }
}

static void onRelayErrorCb(Relay_t relay){
    if(!initEnd){
        return;
    }

    switch(relay){
        case feEn:
        case feDis:
        case feAct:
        case ctEn:
        case ctDis:
        case ctAct:
            RKH_SMA_POST_FIFO(logic, &e_SaltDisable, 0);
            break;
        default:
            break;
    }
}

static void simACb(unsigned char c){
#ifdef DEBUG_SERIAL_PASS
    serialPutByte(UART_DEBUG,c);
#endif
    if(!initEnd){
        return;
    }

    simACmdParser(c);

}

static void simBCb(unsigned char c){
    if(!initEnd){
        return;
    }
}

static void debugCb(unsigned char c){

#ifdef DEBUG_SERIAL_PASS
    //serialPutByte(UART_DEBUG,c);
    serialPutByte(UART_SIM_808_A,c);
#endif
}

static void onMQTTCb(void** state,struct mqtt_response_publish *publish){
    if(!initEnd){
        return;
    }

    char* json = (char*) &(publish->application_message);
    int result = saltCmdParse(json, publish->application_message_size,&(e_saltCmd.cmd));
    if (result > 0){
        RKH_SMA_POST_FIFO(logic, RKH_UPCAST(RKH_EVT_T, &e_saltCmd), 0);
    }

    /*
    char dump1[255] = {0};
    char dump2[255] = {0};

    sprintf(dump1, "MQTT topic: %.*s", publish->topic_name_size, publish->topic_name);
    sprintf(dump2, "MQTT data: %.*s", publish->application_message_size, publish->application_message);

    RKH_TRC_USR_BEGIN(USR_TRACE_MQTT)
        RKH_TUSR_STR(dump1);
        RKH_TUSR_STR(dump2);
    RKH_TRC_USR_END();
    */
}

static void
saltConfig(void)
{
    /* Configuracion especifica SALT */

    /* RKH */

    RKH_SET_STATIC_EVENT(RKH_UPCAST(RKH_EVT_T, &e_saltCmd), evSaltCmd);

    /* Inicializacion SALT */

    bsp_init();
    relayInit(onRelayErrorCb);
    ledPanelInit();
    anInInit(onAnInCb);
    buzzerInit();
    onSwitchInit(onSwitchCb);
    pulseCounterInit(PULSE_COUNTER_THR,PULSE_COUNTER_FACTOR);
    telocInit();

    sim808Init(SIM_808_A);
    serialSetIntCb(UART_SIM_808_A, simACb);

#ifdef DEBUG_SERIAL
    serialInit(UART_DEBUG);
    serialSetIntCb(UART_DEBUG, debugCb);
#else
    sim808Init(SIM_808_B);
    serialSetIntCb(UART_SIM_808_B, simBCb);
#endif

    /* Conexion de modulos */

    simACmdParser = ModCmd_init();

}

static void
setupTraceFilters(void)
{
    RKH_FILTER_ON_GROUP(RKH_TRC_ALL_GROUPS);
    RKH_FILTER_ON_EVENT(RKH_TRC_ALL_EVENTS);
    //RKH_FILTER_OFF_EVENT(USR_TRACE);
    RKH_FILTER_OFF_EVENT(USR_TRACE_OUT);
    //RKH_FILTER_OFF_EVENT(USR_TRACE_EVT);
    RKH_FILTER_OFF_EVENT(USR_TRACE_IN);
    //RKH_FILTER_OFF_EVENT(USR_TRACE_SSP);
    RKH_FILTER_OFF_EVENT(USR_TRACE_MQTT);
    //RKH_FILTER_OFF_GROUP_ALL_EVENTS(RKH_TG_USR);
    //RKH_FILTER_OFF_EVENT(RKH_TE_TMR_TOUT);
    RKH_FILTER_OFF_EVENT(RKH_TE_SM_STATE);
    //RKH_FILTER_OFF_EVENT(RKH_TE_SMA_FIFO);
    //RKH_FILTER_OFF_EVENT(RKH_TE_SMA_LIFO);
    //RKH_FILTER_OFF_EVENT(RKH_TE_SM_TS_STATE);
    //RKH_FILTER_OFF_EVENT(RKH_TE_SM_DCH);
    //RKH_FILTER_OFF_SMA(modMgr);
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
    sprintf(mqttProtCfg.subTopic, "/salt/cmd");
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
    strcpy(mqttProtCfg.subTopic, "");
    mqttProtCfg.callback = onMQTTCb;
    MQTTProt_ctor(&mqttProtCfg, publishDimba);

    logicCfg.publishTime = 8;
    logic_ctor(&logicCfg);

    RKH_SMA_ACTIVATE(conMgr, ConMgr_qsto, CONMGR_QSTO_SIZE, 0, 0);
    RKH_SMA_ACTIVATE(modMgr, ModMgr_qsto, MODMGR_QSTO_SIZE, 0, 0);
    RKH_SMA_ACTIVATE(mqttProt, MQTTProt_qsto, MQTTPROT_QSTO_SIZE, 0, 0);
    RKH_SMA_ACTIVATE(logic, Logic_qsto, LOGIC_QSTO_SIZE, 0, 0);

    RKH_SMA_POST_FIFO(conMgr, &e_Open, 0);

    initEnd = true;

    rkh_fwk_enter();

    RKH_TRC_CLOSE();

    return 0;
}


/* ------------------------------ End of file ------------------------------ */
