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
#define PwrKey_init(sim808)
#define PwrKey(sim808, b)           sim808SetControlPin( (sim808),  SIM_808_PWRKEY, !(b))
#define Power_init(sim808)
#define Power(sim808,b)            sim808SetControlPin( (sim808),  SIM_808_VC, (b))

#define modPwr_toggle(modPwr) \
        { \
            RKH_ENTER_CRITICAL(); \
            modPwr.counter = SIM808_PWR_TIME; \
            modPwr.state = Toggling; \
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

typedef struct ModPwr ModPwr;
struct ModPwr
{
    Sim808_t sim808;
    ModPwrStates state;
    ruint counter;
};

/* ---------------------------- Global variables --------------------------- */
/* ---------------------------- Local variables ---------------------------- */
static ModPwr modPwrTable[] =
    {
        {
            SIM_808_A,
            OnOff,
            0
            },
        {
            SIM_808_B,
            OnOff,
            0
        }
    };

/* ----------------------- Local function prototypes ----------------------- */
/* ---------------------------- Local functions ---------------------------- */
/* ---------------------------- Global functions --------------------------- */
void
modPwr_init(void)
{
    for (int j = 0; j < ModPwrCount; ++j) {
        PwrKey_init(modPwrTable[j].sim808);
        PwrKey(modPwrTable[j].sim808, 1);
        Power(modPwrTable[j].sim808, 0);
        Power_init(modPwrTable[j].sim808);
        modPwrTable[j].state = OnOff;
    }
}

void
modPwr_ctrl(void)
{
    for (int j = 0; j < ModPwrCount; ++j) {
        switch(modPwrTable[j].state)
        {
            case OnOff:
                PwrKey(modPwrTable[j].sim808,1);
                break;

            case Toggling:
                PwrKey(modPwrTable[j].sim808, 0);
                if(modPwrTable[j].counter && (--(modPwrTable[j].counter) == 0))
                {
                    modPwrTable[j].state = OnOff;
                }

                break;
        }
    }

}


void
modPwr_off(ModPwrIndex index)
{
    Power(modPwrTable[index].sim808, 0);
    modPwr_toggle(modPwrTable[index]);
}

void
modPwr_on(ModPwrIndex index)
{
    Power(modPwrTable[index].sim808, 1);
    modPwr_toggle(modPwrTable[index]);
}

/* ------------------------------ End of file ------------------------------ */
