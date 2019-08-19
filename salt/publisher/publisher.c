/**
 *  \file       publisher.c
 *  \brief      Implementation JSON messaje formating and MQTT publish.
 */

/* -------------------------- Development history -------------------------- */
/*
 *  2018.06.22  DaBa  v1.0.00  Initial version
 */

/* -------------------------------- Authors -------------------------------- */
/*
 *  DaBa Dar�o Bali�a  db@vortexmakes.com
 */

/* --------------------------------- Notes --------------------------------- */
/* ----------------------------- Include files ----------------------------- */
#include <string.h>
#include <stdlib.h>
#include "publisher.h"
#include "jWrite.h"
#include "conmgr.h"
#include "logic.h"

/* ----------------------------- Local macros ------------------------------ */
/* ------------------------------- Constants ------------------------------- */
/* ---------------------------- Local data types --------------------------- */
/* ---------------------------- Global variables --------------------------- */
/* ---------------------------- Local variables ---------------------------- */
char dataBuf[1024];
LogicData logicData;

/* ----------------------- Local function prototypes ----------------------- */
/* ---------------------------- Local functions ---------------------------- */
static rui16_t
getNextPublishTime()
{
    return 60;
}

/* ---------------------------- Global functions --------------------------- */
rui16_t
publishDimba(AppData *appMsg)
{
    logic_getData(&logicData);

    jwOpen( dataBuf, sizeof(dataBuf), JW_OBJECT, JW_COMPACT );
    jwObj_int("imei", atoi(ConMgr_imei()));
    jwObj_int(MQTT_PARSE_PARAMETER_CMD_TIMEOUT, logicData.cmdTimeout);
    jwObj_double(MQTT_PARSE_PARAMETER_VEL_CT_ON, logicData.velCtOn);
    jwObj_double(MQTT_PARSE_PARAMETER_VEL_CT_OFF, logicData.velCtOff);
    jwObj_double(MQTT_PARSE_PARAMETER_VEL_FE_ON, logicData.velFeOn);
    jwObj_double(MQTT_PARSE_PARAMETER_TIME_FE_HOLD, logicData.timeFeHold);
    jwObj_double(MQTT_PARSE_PARAMETER_TIME_BLINK_ENABLE, logicData.timeBlinkEnable);
    jwObj_double(MQTT_PARSE_PARAMETER_TIME_BLINK_DISABLE, logicData.timeBlinkDisable);
    jwObj_int(MQTT_PARSE_PARAMETER_PERIOD_BLINK, logicData.blinkPeriod);
    switch(logicData.currentCmd){
        case SALT_CMD_ORDER_STOP:
            jwObj_string(MQTT_PARSE_CMD_KEY, MQTT_PARSE_CMD_STOP);
            break;
        case SALT_CMD_ORDER_DRIFT:
            jwObj_string(MQTT_PARSE_CMD_KEY, MQTT_PARSE_CMD_DRIFT);
            break;
        case SALT_CMD_ORDER_ISOLATED:
            jwObj_string(MQTT_PARSE_CMD_KEY, MQTT_PARSE_CMD_ISOLATED);
            break;
        case SALT_CMD_ORDER_AUTOMATIC:
        default:
            jwObj_string(MQTT_PARSE_CMD_KEY, MQTT_PARSE_CMD_AUTOMATIC);
            break;
    }
    jwObj_double("vel", logicData.vel);
    switch(logicData.velSource){
        case VEL_SOURCE_TELOC:
            jwObj_string("vel_source", "teloc");
            break;
        case VEL_SOURCE_EXTERNAL:
            jwObj_string("vel_source", "external");
            break;
        case VEL_SOURCE_GPS:
            jwObj_string("vel_source", "gps");
            break;
    }
    jwObj_bool("al_mode", logicData.alMode);
    jwClose();

    appMsg->data = (rui8_t *)dataBuf;
    appMsg->size = (rui16_t)strlen(dataBuf);
    return getNextPublishTime();
}

/* ------------------------------ End of file ------------------------------ */
