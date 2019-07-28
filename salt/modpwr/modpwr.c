/**
 *  \file       modpwr_sim900.c
 *  \brief      Implementation of modpwr abstraction for SIM900 on CIAA-NXP.
 */

/* -------------------------- Development history -------------------------- */
/*
 *  2018.06.05  DaBa  v1.0.00  Initial version
 */

/* -------------------------------- Authors -------------------------------- */
/*
 *  DaBa  Dario Bali�a       db@vortexmakes.com
 */

/* --------------------------------- Notes --------------------------------- */
/* ----------------------------- Include files ----------------------------- */
#include "sapi.h"

#include "rkh.h"
#include "modpwr.h"
#include "mTimeCfg.h"
#include "sim808.h"
/* ----------------------------- Local macros ------------------------------ */
#define PwrKey_init()
#define PwrKey(b)           sim808SetControlPin( SIM_808_A,  SIM_808_PWRKEY, !b)
#define Power_init()
#define Power(b)            sim808SetControlPin( SIM_808_A,  SIM_808_VC, b)

#define modPwr_toggle() \
        { \
            RKH_ENTER_CRITICAL(); \
            counter = SIM808_PWR_TIME; \
            state = Toggling; \
            RKH_EXIT_CRITICAL(); \
        }

/* ------------------------------- Constants ------------------------------- */
#define SIM808_PWR_TIME     (1000/MTIME_MODPWR_BASE)

/* ---------------------------- Local data types --------------------------- */
typedef enum ModPwrStates
{
    OnOff,
    Toggling
}ModPwrStates;

/* ---------------------------- Global variables --------------------------- */
/* ---------------------------- Local variables ---------------------------- */
static ruint state, counter;

/* ----------------------- Local function prototypes ----------------------- */
/* ---------------------------- Local functions ---------------------------- */
/* ---------------------------- Global functions --------------------------- */
void
modPwr_init(void)
{
    PwrKey_init();
    PwrKey(1);
    Power_init();
    Power(0);
    state = OnOff;
}

void
modPwr_ctrl(void)
{
    switch(state)
    {
        case OnOff:
            PwrKey(1);
            break;

        case Toggling:
            PwrKey(0);
            if(counter && (--counter == 0))
            {
                state = OnOff;
            }

            break;
    }
}


void
modPwr_off(void)
{
    Power(0);
    modPwr_toggle();
}

void
modPwr_on(void)
{
    Power(1);
    modPwr_toggle();
}

/* ------------------------------ End of file ------------------------------ */
