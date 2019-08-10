/**
 *  \file       modmgr.h
 *  \brief      Module Manager.
 */

/* -------------------------- Development history -------------------------- */
/*
 *  2018.05.02  DaBa  v1.0.00  Initial version
 */

/* -------------------------------- Authors -------------------------------- */
/*
 *  DaBa  Dario Baliña db@vortexmakes.com
 */

/* --------------------------------- Notes --------------------------------- */
/* --------------------------------- Module -------------------------------- */
#ifndef __MODMGR_H__
#define __MODMGR_H__

/* ----------------------------- Include files ----------------------------- */
#include "rkh.h"
#include "modcmd.h"

/* ---------------------- External C language linkage ---------------------- */
#ifdef __cplusplus
extern "C" {
#endif

/* --------------------------------- Macros -------------------------------- */
enum MOD_MGR_INDEX {
    MOD_MGR_A_INDEX, MOD_MGR_B_INDEX,
    NUM_MOD_MGR,
    MOD_MGR_NONE_INDEX
};
#define MOD_MGR_LIST(no_) RKH_ARRAY_SMA(modMgrs, no_)
#define MOD_MGR_A         MOD_MGR_LIST(MOD_MGR_A_INDEX)
#define MOD_MGR_B         MOD_MGR_LIST(MOD_MGR_B_INDEX)

/* -------------------------------- Constants ------------------------------ */
#define MODMGR_MAX_SIZEOF_CMDSTR    70

/* ................................ Signals ................................ */
/* ........................ Declares active object ......................... */

RKH_ARRAY_SMA_DCLR(modMgrs, NUM_MOD_MGR);

/* ------------------------------- Data types ------------------------------ */
typedef struct ModMgrEvt ModMgrEvt;
struct ModMgrEvt
{
    ModCmd args;
    char cmd[MODMGR_MAX_SIZEOF_CMDSTR];
    unsigned char *data;
    ruint nData;
};

typedef struct ModMgrResp ModMgrResp;
struct ModMgrResp
{
    RKH_EVT_T evt;
    RKH_SIG_T fwdEvt;
};

/* -------------------------- External variables --------------------------- */
/* -------------------------- Function prototypes -------------------------- */
void modMgr_ChannelOpen(enum MOD_MGR_INDEX index);
void modMgr_ChannelClose(enum MOD_MGR_INDEX index);
void modMgr_Puts(enum MOD_MGR_INDEX index, char *p);
void modMgr_Putnchar(enum MOD_MGR_INDEX index, unsigned char *p, ruint ndata);

/* -------------------- External C language linkage end -------------------- */
#ifdef __cplusplus
}
#endif

/* ------------------------------ Module end ------------------------------- */
#endif

/* ------------------------------ End of file ------------------------------ */
