/**
 *  \file       logic.h
 *  \brief      Implementation of SAL/T Logic.
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
/* ----------------------------- Include files ----------------------------- */
#include "logic.h"
#include "rkh.h"
#include <string.h>
#include "conmgr.h"
#include "mqttProt.h"
#include "signals.h"
#include "bsp.h"
#include "ledPanel.h"
#include "buzzer.h"
#include "relay.h"

/* ----------------------------- Local macros ------------------------------ */

/* ......................... Declares active object ........................ */
typedef struct Logic Logic;
typedef struct LogicVel LogicVel;

/* ................... Declares states and pseudostates .................... */
RKH_DCLR_BASIC_STATE Logic_Disable, Logic_PreventiveStop, Logic_RemoteStop, Logic_RemoteDrift, Logic_RemoteIsolated,
                    Logic_controlAutomaticEnable, Logic_controlAutomaticDisable, Logic_controlAutomaticBrake,
                    Logic_controlBlinkEnable, Logic_controlBlinkDisable, Logic_controlBlinkBrake,
                    LogicVel_Hasler, LogicVel_External, LogicVel_GPS, LogicVel_Missing;

RKH_DCLR_COMP_STATE Logic_Enable, Logic_remote, Logic_automatic, Logic_controlAutomatic, Logic_controlBlink;
RKH_DCLR_COND_STATE Logic_C1, Logic_C2, Logic_C3, Logic_C4, Logic_C5;


/* ........................ Declares initial action ........................ */
static void init(Logic *const me, RKH_EVT_T *pe);

/* ........................ Declares effect actions ........................ */
static void effect_enable(Logic *const me, RKH_EVT_T *pe);
static void effect_disable(Logic *const me, RKH_EVT_T *pe);
static void effect_gps(Logic *const me, RKH_EVT_T *pe);
static void effect_gpsTout(Logic *const me, RKH_EVT_T *pe);
static void effect_cmd(Logic *const me, RKH_EVT_T *pe);
static void effect_cmdTOut(Logic *const me, RKH_EVT_T *pe);
static void effect_vel(Logic *const me, RKH_EVT_T *pe);

static void effect_velHasler(LogicVel *const me, RKH_EVT_T *pe);
static void effect_velExternal(LogicVel *const me, RKH_EVT_T *pe);
static void effect_velGps(LogicVel *const me, RKH_EVT_T *pe);

/* ......................... Declares entry actions ........................ */
static void entry_disable(Logic *const me);
static void entry_enable(Logic *const me);
static void entry_preventiveStop(Logic *const me);
static void entry_remote(Logic *const me);
static void entry_remoteStop(Logic *const me);
static void entry_remoteDrift(Logic *const me);
static void entry_remoteIsolated(Logic *const me);
static void entry_controlAutomatic(Logic *const me);
static void entry_controlBlink(Logic *const me);
static void entry_controlAutomaticEnable(Logic *const me);
static void entry_controlAutomaticDisable(Logic *const me);
static void entry_controlAutomaticBrake(Logic *const me);
static void entry_controlBlinkEnable(Logic *const me);
static void entry_controlBlinkDisable(Logic *const me);
static void entry_controlBlinkBrake(Logic *const me);


static void entry_hasler(LogicVel *const me);
static void entry_external(LogicVel *const me);
static void entry_gps(LogicVel *const me);
static void entry_missing(LogicVel *const me);

/* ......................... Declares exit actions ......................... */
static void exit_enable(Logic *const me);
static void exit_disable(Logic *const me);
static void exit_preventiveStop(Logic *const me);
static void exit_remote(Logic *const me);
static void exit_remoteStop(Logic *const me);
static void exit_remoteDrift(Logic *const me);
static void exit_remoteIsolated(Logic *const me);
static void exit_controlAutomatic(Logic *const me);
static void exit_controlBlink(Logic *const me);
static void exit_controlAutomaticEnable(Logic *const me);
static void exit_controlAutomaticDisable(Logic *const me);
static void exit_controlAutomaticBrake(Logic *const me);
static void exit_controlBlinkEnable(Logic *const me);
static void exit_controlBlinkDisable(Logic *const me);
static void exit_controlBlinkBrake(Logic *const me);

static void exit_hasler(LogicVel *const me);
static void exit_external(LogicVel *const me);
static void exit_gps(LogicVel *const me);
static void exit_missing(LogicVel *const me);

/* ............................ Declares guards ............................ */
rbool_t guard_velRight(Logic *const me, RKH_EVT_T *pe);
rbool_t guard_velWrong(Logic *const me, RKH_EVT_T *pe);
rbool_t guard_velDisp(Logic *const me, RKH_EVT_T *pe);
rbool_t guard_cmdEnable(Logic *const me, RKH_EVT_T *pe);
rbool_t guard_cmdAutomatic(Logic *const me, RKH_EVT_T *pe);
rbool_t guard_cmdAutomaticNot(Logic *const me, RKH_EVT_T *pe);
rbool_t guard_currentCmdStop(Logic *const me, RKH_EVT_T *pe);
rbool_t guard_currentCmdDrift(Logic *const me, RKH_EVT_T *pe);
rbool_t guard_currentCmdIsolated(Logic *const me, RKH_EVT_T *pe);
rbool_t guard_cmdStopNot(Logic *const me, RKH_EVT_T *pe);
rbool_t guard_cmdDriftNot(Logic *const me, RKH_EVT_T *pe);
rbool_t guard_cmdIsolatedNot(Logic *const me, RKH_EVT_T *pe);
rbool_t guard_VelAboveCTon(Logic *const me, RKH_EVT_T *pe);
rbool_t guard_currentVelAboveCTon(Logic *const me, RKH_EVT_T *pe);
rbool_t guard_VelBelowCToff(Logic *const me, RKH_EVT_T *pe);
rbool_t guard_VelAboveFEon(Logic *const me, RKH_EVT_T *pe);
rbool_t guard_currentVelAboveFEon(Logic *const me, RKH_EVT_T *pe);
rbool_t guard_nextBlinkBrake(Logic *const me, RKH_EVT_T *pe);

rbool_t guard_velRightVel(LogicVel *const me, RKH_EVT_T *pe);
rbool_t guard_velWrongVel(LogicVel *const me, RKH_EVT_T *pe);

/* ........................ States and pseudostates ........................ */
RKH_CREATE_BASIC_STATE(Logic_Disable, entry_disable, exit_disable, RKH_ROOT, NULL);
RKH_CREATE_TRANS_TABLE(Logic_Disable)
                RKH_TRREG(evSaltEnable, NULL, effect_enable, &Logic_Enable),
                RKH_TRINT(evSaltCmd, NULL, effect_cmd),
                RKH_TRINT(evSaltTimeoutCmd, NULL, effect_cmdTOut),
RKH_END_TRANS_TABLE

RKH_CREATE_COMP_REGION_STATE(Logic_Enable, entry_enable, exit_enable, RKH_ROOT,
                             &Logic_C1, NULL,
                             RKH_NO_HISTORY, NULL, NULL, NULL, NULL);
RKH_CREATE_TRANS_TABLE(Logic_Enable)
                RKH_TRREG(evSaltDisable, NULL, effect_disable, &Logic_Disable),
                RKH_TRINT(evVelGPS, NULL, effect_gps),
                RKH_TRINT(evSaltTimeoutGPS, NULL, effect_gpsTout),
                RKH_TRINT(evSaltCmd, NULL, effect_cmd),
                RKH_TRINT(evSaltTimeoutCmd, NULL, effect_cmdTOut),
RKH_END_TRANS_TABLE

RKH_CREATE_COND_STATE(Logic_C1);
RKH_CREATE_BRANCH_TABLE(Logic_C1)
                RKH_BRANCH(guard_velDisp,   NULL,   &Logic_PreventiveStop),
                RKH_BRANCH(ELSE,           NULL,   &Logic_PreventiveStop),
RKH_END_BRANCH_TABLE

RKH_CREATE_BASIC_STATE(Logic_PreventiveStop, entry_preventiveStop, exit_preventiveStop, &Logic_Enable, NULL);
RKH_CREATE_TRANS_TABLE(Logic_PreventiveStop)
                RKH_TRREG(evSaltTimeoutStop, NULL, NULL, &Logic_C1),
RKH_END_TRANS_TABLE

RKH_CREATE_COND_STATE(Logic_C2);
RKH_CREATE_BRANCH_TABLE(Logic_C2)
                RKH_BRANCH(guard_cmdEnable,   NULL,   &Logic_remote),
                RKH_BRANCH(ELSE,           NULL,   &Logic_automatic),
RKH_END_BRANCH_TABLE

RKH_CREATE_COMP_REGION_STATE(Logic_remote, entry_remote, exit_remote, &Logic_Enable,
                             &Logic_C3, NULL,
                             RKH_NO_HISTORY, NULL, NULL, NULL, NULL);
RKH_CREATE_TRANS_TABLE(Logic_remote)
                RKH_TRREG(evSaltCmd, guard_cmdAutomatic, effect_cmd, &Logic_automatic),
                RKH_TRREG(evSaltTimeoutCmd, NULL, effect_cmdTOut, &Logic_automatic),
RKH_END_TRANS_TABLE

RKH_CREATE_COND_STATE(Logic_C3);
RKH_CREATE_BRANCH_TABLE(Logic_C3)
                RKH_BRANCH(guard_currentCmdStop,   NULL,   &Logic_RemoteStop),
                RKH_BRANCH(guard_currentCmdDrift,   NULL,   &Logic_RemoteDrift),
                RKH_BRANCH(guard_currentCmdIsolated,   NULL,   &Logic_RemoteIsolated),
                RKH_BRANCH(ELSE,           NULL,   &Logic_automatic),
RKH_END_BRANCH_TABLE

RKH_CREATE_BASIC_STATE(Logic_RemoteStop, entry_remoteStop, exit_remoteStop, &Logic_remote, NULL);
RKH_CREATE_TRANS_TABLE(Logic_RemoteStop)
                RKH_TRREG(evSaltCmd, guard_cmdStopNot, effect_cmd, &Logic_C3),
RKH_END_TRANS_TABLE

RKH_CREATE_BASIC_STATE(Logic_RemoteDrift, entry_remoteDrift, exit_remoteDrift, &Logic_remote, NULL);
RKH_CREATE_TRANS_TABLE(Logic_RemoteDrift)
                RKH_TRREG(evSaltCmd, guard_cmdDriftNot, effect_cmd, &Logic_C3),
RKH_END_TRANS_TABLE

RKH_CREATE_BASIC_STATE(Logic_RemoteIsolated, entry_remoteIsolated, exit_remoteIsolated, &Logic_remote, NULL);
RKH_CREATE_TRANS_TABLE(Logic_RemoteIsolated)
                RKH_TRREG(evSaltCmd, guard_cmdIsolatedNot, effect_cmd, &Logic_C3),
RKH_END_TRANS_TABLE

RKH_CREATE_COMP_REGION_STATE(Logic_automatic, NULL, NULL, &Logic_Enable,
                             &Logic_C4, NULL,
                             RKH_NO_HISTORY, NULL, NULL, NULL, NULL);
RKH_CREATE_TRANS_TABLE(Logic_automatic)
                RKH_TRREG(evSaltCmd, guard_cmdAutomaticNot, effect_cmd, &Logic_remote),
RKH_END_TRANS_TABLE

RKH_CREATE_COND_STATE(Logic_C4);
RKH_CREATE_BRANCH_TABLE(Logic_C4)
                RKH_BRANCH(guard_velDisp,   NULL,   &Logic_controlAutomatic),
                RKH_BRANCH(ELSE,           NULL,   &Logic_controlBlink),
RKH_END_BRANCH_TABLE

RKH_CREATE_COMP_REGION_STATE(Logic_controlAutomatic, entry_controlAutomatic, exit_controlAutomatic, &Logic_automatic,
                             &Logic_C5, NULL,
                             RKH_NO_HISTORY, NULL, NULL, NULL, NULL);
RKH_CREATE_TRANS_TABLE(Logic_controlAutomatic)
                RKH_TRINT(evVel, guard_velRight, effect_vel),
                RKH_TRREG(evVel, guard_velWrong, NULL, &Logic_controlBlink),
RKH_END_TRANS_TABLE

RKH_CREATE_COND_STATE(Logic_C5);
RKH_CREATE_BRANCH_TABLE(Logic_C5)
                RKH_BRANCH(guard_currentVelAboveFEon,   NULL,   &Logic_controlAutomaticBrake),
                RKH_BRANCH(guard_currentVelAboveCTon,   NULL,   &Logic_controlAutomaticDisable),
                RKH_BRANCH(ELSE,           NULL,   &Logic_controlAutomaticEnable),
RKH_END_BRANCH_TABLE

RKH_CREATE_BASIC_STATE(Logic_controlAutomaticEnable, entry_controlAutomaticEnable, exit_controlAutomaticEnable, &Logic_controlAutomatic, NULL);
RKH_CREATE_TRANS_TABLE(Logic_controlAutomaticEnable)
                RKH_TRREG(evVel, guard_VelAboveFEon, effect_vel, &Logic_controlAutomaticBrake),
                RKH_TRREG(evVel, guard_VelAboveCTon, effect_vel, &Logic_controlAutomaticDisable),
RKH_END_TRANS_TABLE

RKH_CREATE_BASIC_STATE(Logic_controlAutomaticDisable, entry_controlAutomaticDisable, exit_controlAutomaticDisable, &Logic_controlAutomatic, NULL);
RKH_CREATE_TRANS_TABLE(Logic_controlAutomaticDisable)
                RKH_TRREG(evVel, guard_VelAboveFEon, effect_vel, &Logic_controlAutomaticBrake),
                RKH_TRREG(evVel, guard_VelBelowCToff, effect_vel, &Logic_controlAutomaticEnable),
RKH_END_TRANS_TABLE

RKH_CREATE_BASIC_STATE(Logic_controlAutomaticBrake, entry_controlAutomaticBrake, exit_controlAutomaticBrake, &Logic_controlAutomatic, NULL);
RKH_CREATE_TRANS_TABLE(Logic_controlAutomaticBrake)
                RKH_TRREG(evSaltTimeoutStop, guard_currentVelAboveCTon, NULL, &Logic_controlAutomaticDisable),
                RKH_TRREG(evSaltTimeoutStop, NULL, NULL, &Logic_controlAutomaticEnable),
RKH_END_TRANS_TABLE

RKH_CREATE_COMP_REGION_STATE(Logic_controlBlink, entry_controlBlink, exit_controlBlink, &Logic_automatic,
                             &Logic_controlBlinkEnable, NULL,
                             RKH_NO_HISTORY, NULL, NULL, NULL, NULL);
RKH_CREATE_TRANS_TABLE(Logic_controlBlink)
                RKH_TRREG(evVel, guard_velRight, NULL, &Logic_controlAutomatic),
RKH_END_TRANS_TABLE

RKH_CREATE_BASIC_STATE(Logic_controlBlinkEnable, entry_controlBlinkEnable, exit_controlBlinkEnable, &Logic_controlBlink, NULL);
RKH_CREATE_TRANS_TABLE(Logic_controlBlinkEnable)
                RKH_TRREG(evSaltTimeoutStop, NULL, NULL, &Logic_controlBlinkDisable),
RKH_END_TRANS_TABLE

RKH_CREATE_BASIC_STATE(Logic_controlBlinkDisable, entry_controlBlinkDisable, exit_controlBlinkDisable, &Logic_controlBlink, NULL);
RKH_CREATE_TRANS_TABLE(Logic_controlBlinkDisable)
                RKH_TRREG(evSaltTimeoutStop, guard_nextBlinkBrake, NULL, &Logic_controlBlinkBrake),
                RKH_TRREG(evSaltTimeoutStop, NULL, NULL, &Logic_controlBlinkEnable),
RKH_END_TRANS_TABLE

RKH_CREATE_BASIC_STATE(Logic_controlBlinkBrake, entry_controlBlinkBrake, exit_controlBlinkBrake, &Logic_controlBlink, NULL);
RKH_CREATE_TRANS_TABLE(Logic_controlBlinkBrake)
                RKH_TRREG(evSaltTimeoutStop, NULL, NULL, &Logic_controlBlinkEnable),
RKH_END_TRANS_TABLE



RKH_CREATE_BASIC_STATE(LogicVel_Hasler, entry_hasler, exit_hasler, RKH_ROOT, NULL);
RKH_CREATE_TRANS_TABLE(LogicVel_Hasler)
                RKH_TRINT(evVelHasler, guard_velRightVel, effect_velHasler),
                RKH_TRREG(evVelHasler, guard_velWrongVel, NULL, &LogicVel_External),
                RKH_TRREG(evTimeout, NULL, NULL, &LogicVel_External),
RKH_END_TRANS_TABLE

RKH_CREATE_BASIC_STATE(LogicVel_External, entry_external, exit_external, RKH_ROOT, NULL);
RKH_CREATE_TRANS_TABLE(LogicVel_External)
                RKH_TRREG(evVelHasler, guard_velRightVel, effect_velHasler, &LogicVel_Hasler),
                RKH_TRINT(evVelExternal, guard_velRightVel, effect_velExternal),
                RKH_TRREG(evVelExternal, guard_velWrongVel, NULL, &LogicVel_GPS),
                RKH_TRREG(evTimeout, NULL, NULL, &LogicVel_GPS),
RKH_END_TRANS_TABLE

RKH_CREATE_BASIC_STATE(LogicVel_GPS, entry_gps, exit_gps, RKH_ROOT, NULL);
RKH_CREATE_TRANS_TABLE(LogicVel_GPS)
                RKH_TRREG(evVelHasler, guard_velRightVel, effect_velHasler, &LogicVel_Hasler),
                RKH_TRREG(evVelExternal, guard_velRightVel, effect_velExternal, &LogicVel_External),
                RKH_TRINT(evVelGPS, guard_velRightVel, effect_velGps),
                RKH_TRREG(evVelGPS, guard_velWrongVel, NULL, &LogicVel_Missing),
                RKH_TRREG(evTimeout, NULL, NULL, &LogicVel_Missing),
RKH_END_TRANS_TABLE

RKH_CREATE_BASIC_STATE(LogicVel_Missing, entry_missing, exit_missing, RKH_ROOT, NULL);
RKH_CREATE_TRANS_TABLE(LogicVel_Missing)
                RKH_TRREG(evVelHasler, guard_velRightVel, effect_velHasler, &LogicVel_Hasler),
                RKH_TRREG(evVelExternal, guard_velRightVel, effect_velExternal, &LogicVel_External),
                RKH_TRREG(evVelGPS, guard_velRightVel, effect_velGps, &LogicVel_GPS),
RKH_END_TRANS_TABLE


/* ............................. Active object ............................. */

struct LogicVel
{
    RKH_SMA_T ao;       /* base structure */
    Logic *itsLogic;

    RKH_TMR_T timer;

    VelEvt velEvt;
};

struct Logic
{
    RKH_SMA_T ao;       /* base structure */
    RKHSmaVtbl vtbl;
    LogicVel itsLogicVel;

    RKH_TMR_T timerStop;
    RKH_TMR_T timerGpsEnable;
    RKH_TMR_T timerCmdTimeout;

    LedPanelCfg ledConfig;

    SALT_CMD_ORDER currentCmd;
    RKH_TNT_T cmdTimeout;

    double velCtOn;
    double velCtOff;
    double velFeOn;
    double timeFeHold;

    double timeBlinkEnable;
    double timeBlinkDisable;
    rui8_t blinkPeriod;

    rui8_t blinkCount;
};

RKH_SMA_CREATE(Logic, logic, 4, HCAL, &Logic_Disable, init, NULL);
RKH_SMA_DEF_PTR(logic);
RKH_SM_CONST_CREATE(logicVel, 5, HCAL, &LogicVel_Hasler, NULL, NULL);

/* ------------------------------- Constants ------------------------------- */

/* ---------------------------- Local data types --------------------------- */
/* ---------------------------- Global variables --------------------------- */
/* ---------------------------- Local variables ---------------------------- */

static RKH_STATIC_EVENT(e_tOutStop, evSaltTimeoutStop);
static RKH_STATIC_EVENT(e_tOutGps, evSaltTimeoutGPS);
static RKH_STATIC_EVENT(e_tOutCmd, evSaltTimeoutCmd);
static RKH_ROM_STATIC_EVENT(e_timeout, evTimeout);


/* ----------------------- Local function prototypes ----------------------- */
/* ---------------------------- Local functions ---------------------------- */

static void
dispatch(RKH_SMA_T *me, void *arg)
{
    LogicVel *logicVel;

    logicVel = &(RKH_DOWNCAST(Logic, me)->itsLogicVel);
    rkh_sm_dispatch((RKH_SM_T *)me, (RKH_EVT_T *)arg);
    rkh_sm_dispatch(RKH_UPCAST(RKH_SM_T, logicVel), (RKH_EVT_T *)arg);
}

static void
setVelDisplay(Logic *me, VelEvt *velEvt, bool_t on)
{
    if(on){
        if(velEvt != NULL){
            char velStr[10];
            sprintf(velStr,"%5.1f",velEvt->vel);

            me->ledConfig.pointPosition = 2;
            me->ledConfig.digit0 = velStr[0] != ' ' ? velStr[0] : NUM_NULL;
            me->ledConfig.digit1 = velStr[1] != ' ' ? velStr[1] : NUM_NULL;
            me->ledConfig.digit2 = velStr[2] != ' ' ? velStr[2] : NUM_NULL;
            me->ledConfig.digit3 = velStr[4] != ' ' ? velStr[4] : 0;

        } else {
            me->ledConfig.pointPosition = -1;
            me->ledConfig.digit0 = NUM_DASH;
            me->ledConfig.digit1 = NUM_DASH;
            me->ledConfig.digit2 = NUM_DASH;
            me->ledConfig.digit3 = NUM_DASH;
        }
    } else {
        me->ledConfig.pointPosition = -1;
        me->ledConfig.digit0 = NUM_NULL;
        me->ledConfig.digit1 = NUM_NULL;
        me->ledConfig.digit2 = NUM_NULL;
        me->ledConfig.digit3 = NUM_NULL;
    }
    ledPanelSetCfg(&(me->ledConfig));
}

static void configAlCtFe(Logic *me, rbool_t alActivated, rbool_t ctActivated, rbool_t feActivated){

    relaySetCTActive(alActivated ? ctActivated : RKH_FALSE);
    relaySetFEActive(alActivated ? feActivated : RKH_FALSE);
    relaySetAlEnable(alActivated);

    if(alActivated){
        me->ledConfig.ledIsolated = GREEN;
        me->ledConfig.ledCt = ctActivated ? RED : GREEN;
        me->ledConfig.ledFe = feActivated ? RED : GREEN;
    } else {
        me->ledConfig.ledIsolated = LED_OFF;
        me->ledConfig.ledCt = LED_OFF;
        me->ledConfig.ledFe = LED_OFF;
    }

    ledPanelSetCfg(&(me->ledConfig));
}



/* ............................ Initial action ............................. */
static void init(Logic *const me, RKH_EVT_T *pe){
    (void)pe;

    RKH_TR_FWK_AO(me);

    RKH_TR_FWK_TIMER(&me->timerStop);
    RKH_TR_FWK_TIMER(&me->timerGpsEnable);
    RKH_TR_FWK_TIMER(&me->timerCmdTimeout);



    RKH_TR_FWK_STATE(me, &Logic_Disable);
    RKH_TR_FWK_STATE(me, &Logic_PreventiveStop);

    RKH_TR_FWK_STATE(me, &Logic_Enable);

    RKH_TR_FWK_STATE(me, &Logic_C1);

    RKH_TR_FWK_SIG(evSaltEnable);
    RKH_TR_FWK_SIG(evSaltDisable);
    RKH_TR_FWK_SIG(evSaltTimeoutStop);


    RKH_SET_STATIC_EVENT(RKH_UPCAST(RKH_EVT_T, &(me->itsLogicVel.velEvt)), evVel);
    RKH_TMR_INIT(&me->timerStop, &e_tOutStop, NULL);
    RKH_TMR_INIT(&me->timerGpsEnable, &e_tOutGps, NULL);
    RKH_TMR_INIT(&me->timerCmdTimeout, &e_tOutCmd, NULL);


    me->currentCmd = SALT_CMD_ORDER_AUTOMATIC;
    me->cmdTimeout = CMD_TIMEOUT_DEFAULT;
    me->velCtOn = CT_ON_DEFAULT;
    me->velCtOff = CT_OFF_DEFAULT;
    me->velFeOn = FE_ON_DEFAULT;
    me->timeFeHold = T_HOLD_DEFAULT;
    me->timeBlinkEnable = T_BLINK_ENABLE_DEFAULT;
    me->timeBlinkDisable = T_BLINK_DISABLE_DEFAULT;
    me->blinkPeriod = T_BLINK_PERIOD_DEFAULT;
    me->blinkCount = 0;
}

/* ............................ Effect actions ............................. */
static void
effect_enable(Logic *const me, RKH_EVT_T *pe)
{
    (void)pe;
    (void)me;

}

static void
effect_disable(Logic *const me, RKH_EVT_T *pe)
{
    (void)pe;
    (void)me;

}

static void
effect_gps(Logic *const me, RKH_EVT_T *pe)
{
    VelEvt* p = RKH_UPCAST(VelEvt, pe);

    if(p->vel < 0){
        me->ledConfig.ledGps = RED;
    } else {
        me->ledConfig.ledGps = GREEN;
        rkh_tmr_stop(&(me->timerGpsEnable));
        RKH_TMR_ONESHOT(&(me->timerGpsEnable), RKH_UPCAST(RKH_SMA_T, me), GPS_ACTIVE_PERIOD);
    }
    ledPanelSetCfg(&(me->ledConfig));
}

static void
effect_gpsTout(Logic *const me, RKH_EVT_T *pe)
{
    me->ledConfig.ledGps = RED;
    ledPanelSetCfg(&(me->ledConfig));
}

static void effect_cmd(Logic *const me, RKH_EVT_T *pe){
    CmdEvt* p = RKH_UPCAST(CmdEvt, pe);

    switch(p->cmd.type){
        case SALT_CMD_TYPE_CMD:
            me->currentCmd = p->cmd.cmd;
            if(p->cmd.cmd != SALT_CMD_ORDER_AUTOMATIC){
                rkh_tmr_stop(&(me->timerCmdTimeout));
                RKH_TMR_ONESHOT(&(me->timerCmdTimeout), RKH_UPCAST(RKH_SMA_T, me), me->cmdTimeout);
            }
            break;
        case SALT_CMD_TYPE_CONFIG:
            switch (p->cmd.parameter){
                case SALT_PARAMETER_CMD_TIMEOUT:
                    me->cmdTimeout = RKH_TIME_MS(p->cmd.parameterValueDouble);
                    break;
                case SALT_PARAMETER_VEL_CT_ON:
                    me->velCtOn = p->cmd.parameterValueDouble;
                    break;
                case SALT_PARAMETER_VEL_CT_OFF:
                    me->velCtOff = p->cmd.parameterValueDouble;
                    break;
                case SALT_PARAMETER_VEL_FE_ON:
                    me->velFeOn = p->cmd.parameterValueDouble;
                    break;
                case SALT_PARAMETER_TIME_FE_HOLD:
                    me->timeFeHold = RKH_TIME_MS(p->cmd.parameterValueDouble);
                    break;
                case SALT_PARAMETER_TIME_BLINK_ENABLE:
                    me->timeBlinkEnable = RKH_TIME_MS(p->cmd.parameterValueDouble);
                    break;
                case SALT_PARAMETER_TIME_BLINK_DISABLE:
                    me->timeBlinkDisable = RKH_TIME_MS(p->cmd.parameterValueDouble);
                    break;
                case SALT_PARAMETER_PERIOD_BLINK:
                    me->blinkPeriod = RKH_TIME_MS(p->cmd.parameterValueDouble);
                    break;
                default:
                    break;
            }
            break;
    }

}

static void
effect_cmdTOut(Logic *const me, RKH_EVT_T *pe)
{
    me->currentCmd = SALT_CMD_ORDER_AUTOMATIC;
}

static void effect_vel(Logic *const me, RKH_EVT_T *pe){
    VelEvt* velEvt = RKH_DOWNCAST(VelEvt*, pe);

    setVelDisplay(me, velEvt, true);
}

static void effect_velHasler(LogicVel *const me, RKH_EVT_T *pe){

    VelEvt* p = RKH_UPCAST(VelEvt, pe);

    rkh_tmr_stop(&me->timer);
    RKH_TMR_ONESHOT(&me->timer, RKH_UPCAST(RKH_SMA_T, me), WAIT_SOURCE_VEL_PERIOD);

    me->velEvt.source = p->source;
    me->velEvt.vel = p->vel;

    RKH_SMA_POST_FIFO(logic, RKH_UPCAST(RKH_EVT_T, &(me->velEvt)), me);
}

static void effect_velExternal(LogicVel *const me, RKH_EVT_T *pe) {

    VelEvt* p = RKH_UPCAST(VelEvt, pe);

    rkh_tmr_stop(&me->timer);
    RKH_TMR_ONESHOT(&me->timer, RKH_UPCAST(RKH_SMA_T, me), WAIT_SOURCE_VEL_PERIOD);

    me->velEvt.source = p->source;
    me->velEvt.vel = p->vel;

    RKH_SMA_POST_FIFO(logic, RKH_UPCAST(RKH_EVT_T, &(me->velEvt)), me);

}

static void effect_velGps(LogicVel *const me, RKH_EVT_T *pe){

    VelEvt* p = RKH_UPCAST(VelEvt, pe);

    rkh_tmr_stop(&me->timer);
    RKH_TMR_ONESHOT(&me->timer, RKH_UPCAST(RKH_SMA_T, me), WAIT_SOURCE_VEL_PERIOD);

    me->velEvt.source = p->source;
    me->velEvt.vel = p->vel;

    RKH_SMA_POST_FIFO(logic, RKH_UPCAST(RKH_EVT_T, &(me->velEvt)), me);

}

/* ............................. Entry actions ............................. */
static void
entry_disable(Logic *const me)
{

    configAlCtFe(me, RKH_FALSE, RKH_FALSE, RKH_FALSE);
    setVelDisplay(me, NULL, false);
    me->ledConfig.ledOn = GREEN;
    me->ledConfig.ledGps = LED_OFF;
    me->ledConfig.ledRemoteOp = LED_OFF;

    ledPanelSetCfg(&(me->ledConfig));
    //buzzerSet(false);

}

static void
entry_enable(Logic *const me)
{
    (void)me;

    configAlCtFe(me, RKH_TRUE, RKH_FALSE, RKH_FALSE);
    me->ledConfig.ledGps = RED;
    me->ledConfig.ledRemoteOp = RED;

    ledPanelSetCfg(&(me->ledConfig));
}

static void
entry_preventiveStop(Logic *const me)
{
    (void)me;

    configAlCtFe(me, RKH_TRUE,RKH_TRUE,RKH_TRUE);

    rkh_tmr_stop(&me->timerStop);
    RKH_TMR_ONESHOT(&me->timerStop, RKH_UPCAST(RKH_SMA_T, me), PREVENTIVE_STOP_PERIOD);
}

static void entry_remote(Logic *const me){
    me->ledConfig.ledRemoteOp = GREEN;
    ledPanelSetCfg(&(me->ledConfig));
}

static void entry_remoteStop(Logic *const me){
    configAlCtFe(me, RKH_TRUE,RKH_TRUE,RKH_TRUE);
}

static void entry_remoteDrift(Logic *const me){
    configAlCtFe(me, RKH_TRUE,RKH_TRUE,RKH_FALSE);
}

static void entry_remoteIsolated(Logic *const me){
    configAlCtFe(me, RKH_TRUE,RKH_FALSE,RKH_FALSE);
}

static void entry_controlAutomatic(Logic *const me){
    VelEvt* velEvt = &(me->itsLogicVel.velEvt);
    setVelDisplay(me, velEvt, true);
}

static void entry_controlBlink(Logic *const me){
    setVelDisplay(me, NULL, true);
    me->blinkCount = 0;
}

static void entry_controlAutomaticEnable(Logic *const me){
    configAlCtFe(me, RKH_TRUE, RKH_FALSE, RKH_FALSE);
}

static void entry_controlAutomaticDisable(Logic *const me){
    configAlCtFe(me, RKH_TRUE, RKH_TRUE, RKH_FALSE);
    buzzerSetMode(BUZZER_MODE_BLINK);
}

static void entry_controlAutomaticBrake(Logic *const me){
    configAlCtFe(me, RKH_TRUE, RKH_TRUE, RKH_TRUE);
    buzzerSetMode(BUZZER_MODE_CONSTANT);
    rkh_tmr_stop(&me->timerStop);
    RKH_TMR_ONESHOT(&me->timerStop, RKH_UPCAST(RKH_SMA_T, me), RKH_TIME_MS(me->timeFeHold));
}

static void entry_controlBlinkEnable(Logic *const me){
    configAlCtFe(me, RKH_TRUE, RKH_FALSE, RKH_FALSE);
    rkh_tmr_stop(&me->timerStop);
    RKH_TMR_ONESHOT(&me->timerStop, RKH_UPCAST(RKH_SMA_T, me), RKH_TIME_MS(me->timeBlinkEnable));
}

static void entry_controlBlinkDisable(Logic *const me){
    configAlCtFe(me, RKH_TRUE, RKH_TRUE, RKH_FALSE);
    rkh_tmr_stop(&me->timerStop);
    RKH_TMR_ONESHOT(&me->timerStop, RKH_UPCAST(RKH_SMA_T, me), RKH_TIME_MS(me->timeBlinkDisable));
}

static void entry_controlBlinkBrake(Logic *const me){
    configAlCtFe(me, RKH_TRUE, RKH_TRUE, RKH_TRUE);
    rkh_tmr_stop(&me->timerStop);
    RKH_TMR_ONESHOT(&me->timerStop, RKH_UPCAST(RKH_SMA_T, me), RKH_TIME_MS(me->timeFeHold));
}

static void entry_hasler(LogicVel *const me){
    RKH_TMR_INIT(&me->timer, &e_timeout, NULL);
    rkh_tmr_stop(&me->timer);
    RKH_TMR_ONESHOT(&me->timer, RKH_UPCAST(RKH_SMA_T, me), WAIT_SOURCE_VEL_PERIOD);
}

static void entry_external(LogicVel *const me){
    RKH_TMR_INIT(&me->timer, &e_timeout, NULL);
    rkh_tmr_stop(&me->timer);
    RKH_TMR_ONESHOT(&me->timer, RKH_UPCAST(RKH_SMA_T, me), WAIT_SOURCE_VEL_PERIOD);
}

static void entry_gps(LogicVel *const me){
    RKH_TMR_INIT(&me->timer, &e_timeout, NULL);
    rkh_tmr_stop(&me->timer);
    RKH_TMR_ONESHOT(&me->timer, RKH_UPCAST(RKH_SMA_T, me), WAIT_SOURCE_VEL_PERIOD);
}

static void entry_missing(LogicVel *const me){
    me->velEvt.vel = -1;
    RKH_SMA_POST_FIFO(logic, RKH_UPCAST(RKH_EVT_T, &(me->velEvt)), me);
}

/* ............................. Exit actions ............................. */
static void
exit_enable(Logic *const me)
{
    (void)me;

    rkh_tmr_stop(&me->timerGpsEnable);
    rkh_tmr_stop(&me->timerStop);
}

static void
exit_disable(Logic *const me)
{

}
static void
exit_preventiveStop(Logic *const me)
{
    (void)me;
    configAlCtFe(me, RKH_TRUE,RKH_FALSE,RKH_FALSE);
}

static void exit_remote(Logic *const me){
    me->ledConfig.ledRemoteOp = RED;
    ledPanelSetCfg(&(me->ledConfig));
}

static void exit_remoteStop(Logic *const me){
    configAlCtFe(me, RKH_TRUE,RKH_FALSE,RKH_FALSE);
}

static void exit_remoteDrift(Logic *const me){
    configAlCtFe(me, RKH_TRUE,RKH_FALSE,RKH_FALSE);
}

static void exit_remoteIsolated(Logic *const me){
    configAlCtFe(me, RKH_TRUE,RKH_FALSE,RKH_FALSE);
}

static void exit_controlAutomatic(Logic *const me){

}

static void exit_controlBlink(Logic *const me){

}

static void exit_controlAutomaticEnable(Logic *const me){

}

static void exit_controlAutomaticDisable(Logic *const me){
    buzzerSetMode(BUZZER_MODE_DISABLE);
}

static void exit_controlAutomaticBrake(Logic *const me){
    buzzerSetMode(BUZZER_MODE_DISABLE);
}

static void exit_controlBlinkEnable(Logic *const me){

}

static void exit_controlBlinkDisable(Logic *const me){

}

static void exit_controlBlinkBrake(Logic *const me){

}


static void exit_hasler(LogicVel *const me){
    rkh_tmr_stop(&me->timer);
}

static void exit_external(LogicVel *const me){
    rkh_tmr_stop(&me->timer);
}

static void exit_gps(LogicVel *const me){
    rkh_tmr_stop(&me->timer);
}

static void exit_missing(LogicVel *const me){

}

/* ................................ Guards ................................. */

rbool_t guard_velRight(Logic *const me, RKH_EVT_T *pe){
    return guard_velRightVel(&(me->itsLogicVel), RKH_UPCAST(RKH_EVT_T,pe));
}

rbool_t guard_velWrong(Logic *const me, RKH_EVT_T *pe){
    return guard_velWrongVel(&(me->itsLogicVel), RKH_UPCAST(RKH_EVT_T,pe));
}

rbool_t
guard_velDisp(Logic *const me, RKH_EVT_T *pe)
{
    (void)pe;

    return guard_velRight(me, RKH_UPCAST(RKH_EVT_T,&(me->itsLogicVel.velEvt)));
}

rbool_t guard_cmdEnable(Logic *const me, RKH_EVT_T *pe){
    switch (me->currentCmd){
        case SALT_CMD_ORDER_STOP:
        case SALT_CMD_ORDER_DRIFT:
        case SALT_CMD_ORDER_ISOLATED:
            return true;
        default:
            break;
    }
    return false;
}

rbool_t guard_cmdAutomatic(Logic *const me, RKH_EVT_T *pe){
    CmdEvt* p = RKH_UPCAST(CmdEvt, pe);
    return p->cmd.type == SALT_CMD_TYPE_CMD && p->cmd.cmd == SALT_CMD_ORDER_AUTOMATIC;
}

rbool_t guard_cmdAutomaticNot(Logic *const me, RKH_EVT_T *pe){
    return !guard_cmdAutomatic(me,pe);
}

rbool_t guard_currentCmdStop(Logic *const me, RKH_EVT_T *pe){
    return me->currentCmd == SALT_CMD_ORDER_STOP;
}

rbool_t guard_currentCmdDrift(Logic *const me, RKH_EVT_T *pe){
    return me->currentCmd == SALT_CMD_ORDER_DRIFT;
}

rbool_t guard_currentCmdIsolated(Logic *const me, RKH_EVT_T *pe){
    return me->currentCmd == SALT_CMD_ORDER_ISOLATED;
}

rbool_t guard_cmdStopNot(Logic *const me, RKH_EVT_T *pe){
    CmdEvt* p = RKH_UPCAST(CmdEvt, pe);
    return p->cmd.type == SALT_CMD_TYPE_CMD && p->cmd.cmd != SALT_CMD_ORDER_STOP;
}

rbool_t guard_cmdDriftNot(Logic *const me, RKH_EVT_T *pe){
    CmdEvt* p = RKH_UPCAST(CmdEvt, pe);
    return p->cmd.type == SALT_CMD_TYPE_CMD && p->cmd.cmd != SALT_CMD_ORDER_DRIFT;
}

rbool_t guard_cmdIsolatedNot(Logic *const me, RKH_EVT_T *pe){
    CmdEvt* p = RKH_UPCAST(CmdEvt, pe);
    return p->cmd.type == SALT_CMD_TYPE_CMD && p->cmd.cmd != SALT_CMD_ORDER_ISOLATED;
}

rbool_t guard_VelAboveCTon(Logic *const me, RKH_EVT_T *pe){
    VelEvt* p = RKH_UPCAST(VelEvt, pe);
    return p->vel > me->velCtOn;
}

rbool_t guard_currentVelAboveCTon(Logic *const me, RKH_EVT_T *pe){
    return guard_VelAboveCTon(me, RKH_UPCAST(RKH_EVT_T,&(me->itsLogicVel.velEvt)));
}

rbool_t guard_VelBelowCToff(Logic *const me, RKH_EVT_T *pe){
    VelEvt* p = RKH_UPCAST(VelEvt, pe);
    return p->vel < me->velCtOff;
}

rbool_t guard_VelAboveFEon(Logic *const me, RKH_EVT_T *pe){
    VelEvt* p = RKH_UPCAST(VelEvt, pe);
    return p->vel > me->velFeOn;
}

rbool_t guard_currentVelAboveFEon(Logic *const me, RKH_EVT_T *pe){
    return guard_VelAboveFEon(me, RKH_UPCAST(RKH_EVT_T,&(me->itsLogicVel.velEvt)));
}

rbool_t guard_nextBlinkBrake(Logic *const me, RKH_EVT_T *pe){
    me->blinkCount += 1;
    if(me->blinkCount >= me->blinkPeriod){
        me->blinkCount = 0;
        return true;
    }
    return false;
}

rbool_t guard_velRightVel(LogicVel *const me, RKH_EVT_T *pe){

    VelEvt* p = RKH_UPCAST(VelEvt, pe);

    return p->vel >= 0;
}

rbool_t guard_velWrongVel(LogicVel *const me, RKH_EVT_T *pe){
    return !guard_velRightVel(me,pe);
}

/* ---------------------------- Global functions --------------------------- */
void logic_ctor(LogicCfg *config){
    Logic *me;

    me = RKH_DOWNCAST(Logic, logic);
    me->vtbl = rkhSmaVtbl;
    me->vtbl.task = dispatch;
    rkh_sma_ctor(RKH_UPCAST(RKH_SMA_T, me), &me->vtbl);

    me->itsLogicVel.itsLogic = me;
    RKH_SM_INIT((RKH_SM_T *)&(me->itsLogicVel), logicVel, 0, HCAL,
                LogicVel_Hasler, NULL, NULL);
}

void logic_getData(LogicData* data){
    Logic *me = RKH_DOWNCAST(Logic, logic);
    data->cmdTimeout = me->cmdTimeout;
    data->blinkPeriod = me->blinkPeriod;
    data->timeBlinkDisable = me->timeBlinkDisable;
    data->timeBlinkEnable = me->timeBlinkEnable;
    data->timeFeHold = me->timeFeHold;
    data->velCtOn = me->velCtOn;
    data->velCtOff = me->velCtOff;
    data->velFeOn = me->velFeOn;
    data->currentCmd = me->currentCmd;
    data->vel = me->itsLogicVel.velEvt.vel;
    data->velSource = me->itsLogicVel.velEvt.source;
    data->alMode = me->ledConfig.ledIsolated == GREEN;
}

/* ------------------------------ End of file ------------------------------ */