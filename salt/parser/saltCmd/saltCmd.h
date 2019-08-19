/**
 *  \file       saltCmd.h
 *  \brief      saltCmd parser definition.
 */

/* -------------------------- Development history -------------------------- */
/*
 *  2019.06.17  IMD  v1.0.00  SALT changes
 *
 */

/* -------------------------------- Authors -------------------------------- */
/*
 *  IMD  Ivan Mariano Di Vito divitoivan@gmail.com.com
 */

/* --------------------------------- Notes --------------------------------- */
/* --------------------------------- Module -------------------------------- */
#ifndef __SALT_CMD_H__
#define __SALT_CMD_H__

/* ----------------------------- Include files ----------------------------- */
#include "rkh.h"
#include "sapi.h"
/* ---------------------- External C language linkage ---------------------- */
#ifdef __cplusplus
extern "C" {
#endif

/* --------------------------------- Macros -------------------------------- */
/* -------------------------------- Constants ------------------------------ */
#define MQTT_PARSER_MAX_TOKENS                  128

#define MQTT_PARSE_TYPE_KEY                     "type"
#define MQTT_PARSE_TYPE_CMD                     "cmd"
#define MQTT_PARSE_TYPE_CONFIG                  "config"

#define MQTT_PARSE_CMD_KEY                      "cmd"
#define MQTT_PARSE_CMD_STOP                     "total_stop"
#define MQTT_PARSE_CMD_DRIFT                    "drift"
#define MQTT_PARSE_CMD_ISOLATED                 "total_isolated"
#define MQTT_PARSE_CMD_AUTOMATIC                "automatic"

#define MQTT_PARSE_PARAMETER_KEY                "parameter"
#define MQTT_PARSE_PARAMETER_CMD_TIMEOUT        "cmd_timeout"
#define MQTT_PARSE_PARAMETER_VEL_CT_ON          "vel_ct_on"
#define MQTT_PARSE_PARAMETER_VEL_CT_OFF         "vel_ct_off"
#define MQTT_PARSE_PARAMETER_VEL_FE_ON          "vel_fe_on"
#define MQTT_PARSE_PARAMETER_TIME_FE_HOLD       "time_fe_hold"
#define MQTT_PARSE_PARAMETER_TIME_BLINK_ENABLE  "time_blink_enable"
#define MQTT_PARSE_PARAMETER_TIME_BLINK_DISABLE "time_blink_disable"
#define MQTT_PARSE_PARAMETER_PERIOD_BLINK       "period_blink"

#define MQTT_PARSE_VALUE_KEY                "value"
/* ------------------------------- Data types ------------------------------ */
typedef enum SALT_CMD_TYPE
{
    SALT_CMD_TYPE_CMD,
    SALT_CMD_TYPE_CONFIG,
} SALT_CMD_TYPE;

typedef enum SALT_CMD_ORDER
{
    SALT_CMD_ORDER_STOP,
    SALT_CMD_ORDER_DRIFT,
    SALT_CMD_ORDER_ISOLATED,
    SALT_CMD_ORDER_AUTOMATIC,
    SALT_CMD_ORDER_COUNT,
    SALT_CMD_ORDER_NULL,
} SALT_CMD_ORDER;

typedef enum SALT_PARAMETER
{
    SALT_PARAMETER_CMD_TIMEOUT,
    SALT_PARAMETER_VEL_CT_ON,
    SALT_PARAMETER_VEL_CT_OFF,
    SALT_PARAMETER_VEL_FE_ON,
    SALT_PARAMETER_TIME_FE_HOLD,
    SALT_PARAMETER_TIME_BLINK_ENABLE,
    SALT_PARAMETER_TIME_BLINK_DISABLE,
    SALT_PARAMETER_PERIOD_BLINK,
} SALT_PARAMETER;

typedef struct SaltCmd
{
    SALT_CMD_TYPE type;
    SALT_CMD_ORDER cmd;
    SALT_PARAMETER parameter;
    char * parameterValueString;
    size_t parameterValueStringSize;
    double parameterValueDouble;
    bool_t parameterValueBool;
} SaltCmd;
/* -------------------------- External variables --------------------------- */
/* -------------------------- Function prototypes -------------------------- */
int saltCmdParse(char* json, size_t jsonSize, SaltCmd* result);

/* -------------------- External C language linkage end -------------------- */
#ifdef __cplusplus
}
#endif

/* ------------------------------ Module end ------------------------------- */
#endif

/* ------------------------------ End of file ------------------------------ */
