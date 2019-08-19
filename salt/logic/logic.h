/**
 *  \file       logic.h
 *  \brief      Specification of SAL/T Logic.
 */

/* -------------------------- Development history -------------------------- */
/*
 *  2018.08.17  IMD  v1.0.01  Initial version
 */

/* -------------------------------- Authors -------------------------------- */
/*
 *  IMD  Ivan Mariano Di Vito      divitoivan@gmail.com
 */

/* --------------------------------- Notes --------------------------------- */
/* --------------------------------- Module -------------------------------- */
#ifndef __LOGIC_H__
#define __LOGIC_H__

/* ----------------------------- Include files ----------------------------- */
#include "rkh.h"
#include "gps.h"
#include "saltCmd.h"

/* ---------------------- External C language linkage ---------------------- */
#ifdef __cplusplus
extern "C" {
#endif

/* --------------------------------- Macros -------------------------------- */
/* -------------------------------- Constants ------------------------------ */
/**
 * Specifies time of preventive stop.
 */
#define PREVENTIVE_STOP_PERIOD   RKH_TIME_MS(5000)

/**
 * Specifies waiting time of a source.
 */
#define WAIT_SOURCE_VEL_PERIOD   RKH_TIME_MS(5000)

/**
 * Specifies time of gps active link.
 */
#define GPS_ACTIVE_PERIOD   RKH_TIME_MS(2000)

/**
 * Specifies time of gps active link.
 */
#define CMD_TIMEOUT_DEFAULT   RKH_TIME_MS(5000)

/**
 * Specifies default CT on vel.
 */
#define CT_ON_DEFAULT   30.0

/**
 * Specifies default CT off vel.
 */
#define CT_OFF_DEFAULT   25.0

/**
 * Specifies default FE on vel.
 */
#define FE_ON_DEFAULT   36.0

/**
 * Specifies default FE hold time.
 */
#define T_HOLD_DEFAULT   30.0

/**
 * Specifies default Blink enable time.
 */
#define T_BLINK_ENABLE_DEFAULT   5.0

/**
 * Specifies default Blink disable time.
 */
#define T_BLINK_DISABLE_DEFAULT   10.0

/**
 * Specifies default Blink cycle period.
 */
#define T_BLINK_PERIOD_DEFAULT   5

/* ................................ Signals ................................ */
/* ........................ Declares active object ......................... */
RKH_SMA_DCLR(logic);

/* ------------------------------- Data types ------------------------------ */

typedef struct LogicCfg LogicCfg;
struct LogicCfg
{
    rui16_t publishTime;    /* in secs */
};

typedef enum VEL_SOURCE{
    VEL_SOURCE_TELOC,
    VEL_SOURCE_EXTERNAL,
    VEL_SOURCE_GPS,
}VEL_SOURCE;

typedef struct VelEvt VelEvt;
struct VelEvt
{
    RKH_EVT_T evt;
    float vel;
    VEL_SOURCE source;
};

typedef struct CmdEvt CmdEvt;
struct CmdEvt
{
    RKH_EVT_T evt;
    SaltCmd cmd;
};

typedef struct LogicData
{
    RKH_TNT_T cmdTimeout;
    double velCtOn;
    double velCtOff;
    double velFeOn;
    double timeFeHold;

    double timeBlinkEnable;
    double timeBlinkDisable;
    rui8_t blinkPeriod;

    SALT_CMD_ORDER currentCmd;
    float vel;
    VEL_SOURCE velSource;
    rbool_t alMode;
} LogicData;

/* -------------------------- External variables --------------------------- */
/* -------------------------- Function prototypes -------------------------- */
void logic_ctor(LogicCfg *config);
void logic_getData(LogicData* data);

/* -------------------- External C language linkage end -------------------- */
#ifdef __cplusplus
}
#endif

/* ------------------------------ Module end ------------------------------- */
#endif

/* ------------------------------ End of file ------------------------------ */