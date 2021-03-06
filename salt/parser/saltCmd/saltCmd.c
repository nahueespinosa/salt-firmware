/**
 *  \file       saltCmd.c
 *  \brief      saltCmd parser Implementation.
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
#include "saltCmd.h"
#include "logic.h"
#include "signals.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define JSMN_PARENT_LINKS
#include "jsmn.h"
#include "bsp.h"

/* ----------------------------- Local macros ------------------------------ */
/* ------------------------------- Constants ------------------------------- */

//#define LOG_PARSE

/* ---------------------------- Local data types --------------------------- */
/* ---------------------------- Global variables --------------------------- */
/* ---------------------------- Local variables ---------------------------- */
static jsmn_parser mqttParser;
static jsmntok_t mqttParserTokens[MQTT_PARSER_MAX_TOKENS];

/* ----------------------- Local function prototypes ----------------------- */
/* ---------------------------- Local functions ---------------------------- */

/* ---------------------------- Global functions --------------------------- */

int saltCmdParse(char* json, size_t jsonSize, SaltCmd* result){

    jsmn_init(&mqttParser);
    int tokens = jsmn_parse(&mqttParser, json, jsonSize, mqttParserTokens, MQTT_PARSER_MAX_TOKENS);
    if(tokens < 0){
        return tokens;
    }

    result->type = SALT_CMD_TYPE_NULL;
    result->cmd = SALT_CMD_ORDER_NULL;
    result->parameter = SALT_PARAMETER_NULL;
    result->parameterValueString = NULL;
    result->parameterValueStringSize = 0;
    result->parameterValueDouble = 0.0;
    result->parameterValueBool = false;

#ifdef LOG_PARSE
    char dump1[255] = {0};
#endif

    for (int i = 0; i < tokens; ++i) {
        jsmntok_t * tok = &(mqttParserTokens[i]);
        if(tok->type == JSMN_STRING){
#ifdef LOG_PARSE
            sprintf(dump1, "data: %.*s", tok->end - tok->start, &(json[tok->start]));
            RKH_TRC_USR_BEGIN(USR_TRACE_MQTT)
                RKH_TUSR_STR("parse");
                RKH_TUSR_STR(dump1);
            RKH_TRC_USR_END();
#endif
            if(strncmp(MQTT_PARSE_TYPE_KEY, &(json[tok->start]), tok->end - tok->start) == 0){
                if(tok->size != 1){
                    return JSMN_ERROR_INVAL;
                }
                jsmntok_t * typeTok = &(mqttParserTokens[i+1]);
#ifdef LOG_PARSE
                sprintf(dump1, "data: %.*s", typeTok->end - typeTok->start, &(json[typeTok->start]));
                RKH_TRC_USR_BEGIN(USR_TRACE_MQTT)
                    RKH_TUSR_STR("type");
                    RKH_TUSR_STR(dump1);
                RKH_TRC_USR_END();
#endif
                if(strncmp(MQTT_PARSE_TYPE_CMD, &(json[typeTok->start]), typeTok->end - typeTok->start) == 0){
                    result->type = SALT_CMD_TYPE_CMD;
                } else if (strncmp(MQTT_PARSE_TYPE_CONFIG, &(json[typeTok->start]), typeTok->end - typeTok->start) == 0){
                    result->type = SALT_CMD_TYPE_CONFIG;
#ifdef LOG_PARSE
                    RKH_TRC_USR_BEGIN(USR_TRACE_MQTT)
                        RKH_TUSR_STR("config");
                    RKH_TRC_USR_END();
#endif
                } else {
                    return JSMN_ERROR_INVAL;
                }
            } else if (strncmp(MQTT_PARSE_CMD_KEY, &(json[tok->start]), tok->end - tok->start) == 0){
                if(tok->size != 1){
                    return JSMN_ERROR_INVAL;
                }
                jsmntok_t * cmdTok = &(mqttParserTokens[i+1]);
#ifdef LOG_PARSE
                sprintf(dump1, "data: %.*s", cmdTok->end - cmdTok->start, &(json[cmdTok->start]));
                RKH_TRC_USR_BEGIN(USR_TRACE_MQTT)
                    RKH_TUSR_STR("cmd");
                    RKH_TUSR_STR(dump1);
                RKH_TRC_USR_END();
#endif
                if(strncmp(MQTT_PARSE_CMD_STOP, &(json[cmdTok->start]), cmdTok->end - cmdTok->start) == 0){
                    result->cmd = SALT_CMD_ORDER_STOP;
                } else if (strncmp(MQTT_PARSE_CMD_DRIFT, &(json[cmdTok->start]), cmdTok->end - cmdTok->start) == 0){
                    result->cmd = SALT_CMD_ORDER_DRIFT;
                } else if (strncmp(MQTT_PARSE_CMD_ISOLATED, &(json[cmdTok->start]), cmdTok->end - cmdTok->start) == 0){
                    result->cmd = SALT_CMD_ORDER_ISOLATED;
                } else if (strncmp(MQTT_PARSE_CMD_AUTOMATIC, &(json[cmdTok->start]), cmdTok->end - cmdTok->start) == 0){
                    result->cmd = SALT_CMD_ORDER_AUTOMATIC;
                } else {
                    return JSMN_ERROR_INVAL;
                }
            } else if (strncmp(MQTT_PARSE_PARAMETER_KEY, &(json[tok->start]), tok->end - tok->start) == 0){
                if(tok->size != 1){
                    return JSMN_ERROR_INVAL;
                }
                jsmntok_t * parameterTok = &(mqttParserTokens[i+1]);
#ifdef LOG_PARSE
                sprintf(dump1, "data: %.*s", parameterTok->end - parameterTok->start, &(json[parameterTok->start]));
                RKH_TRC_USR_BEGIN(USR_TRACE_MQTT)
                    RKH_TUSR_STR("parameter");
                    RKH_TUSR_STR(dump1);
                RKH_TRC_USR_END();
#endif
                if(strncmp(MQTT_PARSE_PARAMETER_CMD_TIMEOUT, &(json[parameterTok->start]), parameterTok->end - parameterTok->start) == 0){
                    result->parameter = SALT_PARAMETER_CMD_TIMEOUT;
                } else if(strncmp(MQTT_PARSE_PARAMETER_VEL_CT_ON, &(json[parameterTok->start]), parameterTok->end - parameterTok->start) == 0){
                    result->parameter = SALT_PARAMETER_VEL_CT_ON;
                } else if(strncmp(MQTT_PARSE_PARAMETER_VEL_CT_OFF, &(json[parameterTok->start]), parameterTok->end - parameterTok->start) == 0){
                    result->parameter = SALT_PARAMETER_VEL_CT_OFF;
                } else if(strncmp(MQTT_PARSE_PARAMETER_VEL_FE_ON, &(json[parameterTok->start]), parameterTok->end - parameterTok->start) == 0){
                    result->parameter = SALT_PARAMETER_VEL_FE_ON;
                } else if(strncmp(MQTT_PARSE_PARAMETER_TIME_FE_HOLD, &(json[parameterTok->start]), parameterTok->end - parameterTok->start) == 0){
                    result->parameter = SALT_PARAMETER_TIME_FE_HOLD;
                } else if(strncmp(MQTT_PARSE_PARAMETER_TIME_BLINK_ENABLE, &(json[parameterTok->start]), parameterTok->end - parameterTok->start) == 0){
                    result->parameter = SALT_PARAMETER_TIME_BLINK_DISABLE;
                } else if(strncmp(MQTT_PARSE_PARAMETER_TIME_BLINK_DISABLE, &(json[parameterTok->start]), parameterTok->end - parameterTok->start) == 0){
                    result->parameter = SALT_PARAMETER_TIME_BLINK_DISABLE;
                } else if(strncmp(MQTT_PARSE_PARAMETER_PERIOD_BLINK, &(json[parameterTok->start]), parameterTok->end - parameterTok->start) == 0){
                    result->parameter = SALT_PARAMETER_PERIOD_BLINK;
                } else if(strncmp(MQTT_PARSE_PARAMETER_PUBLISH_PERIOD, &(json[parameterTok->start]), parameterTok->end - parameterTok->start) == 0){
                    result->parameter = SALT_PARAMETER_PUBLISH_PERIOD;
                } else {
                    return JSMN_ERROR_INVAL;
                }


            } else if (strncmp(MQTT_PARSE_VALUE_KEY, &(json[tok->start]), tok->end - tok->start) == 0){
                if(tok->size != 1){
                    return JSMN_ERROR_INVAL;
                }
                jsmntok_t * valueTok = &(mqttParserTokens[i+1]);
                char* data = &(json[valueTok->start]);
                switch (valueTok->type){
                    case JSMN_STRING:
                        result->parameterValueString = data;
                        result->parameterValueStringSize = valueTok->end - valueTok->start;
                        break;
                    case JSMN_PRIMITIVE:
                        switch (*data){
                            case 't':
                                result->parameterValueBool = true;
                                break;
                            case 'f':
                                result->parameterValueBool = false;
                                break;
                            case '-':
                            case '0':
                            case '1':
                            case '2':
                            case '3':
                            case '4':
                            case '5':
                            case '6':
                            case '7':
                            case '8':
                            case '9':
                                result->parameterValueDouble = strtod(data, NULL);
                                break;
                            default:
                                return JSMN_ERROR_INVAL;
                        }
                        break;
                    default:
                        return JSMN_ERROR_INVAL;
                }
            }
            i+=tok->size;
        }

    }
    return tokens;
}

/* ------------------------------ End of file ------------------------------ */
