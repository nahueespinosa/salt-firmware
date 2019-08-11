/**
 *  \file       sim900parser.h
 *  \brief      SIM900 parser for SSP.
 */

/* -------------------------- Development history -------------------------- */
/*
 *  2018.05.07  LeFr  v1.0.00  Initial version
 */

/* -------------------------------- Authors -------------------------------- */
/*
 *  LeFr  Leandro Francucci lf@vortexmakes.com
 */

/* --------------------------------- Notes --------------------------------- */
/* --------------------------------- Module -------------------------------- */
#ifndef __SIM900PARSER_H__
#define __SIM900PARSER_H__

/* ----------------------------- Include files ----------------------------- */
#include "rkh.h"
#include "ssp.h"
#include "modmgr.h"
#include "conmgr.h"

/* ---------------------- External C language linkage ---------------------- */
#ifdef __cplusplus
extern "C" {
#endif

/* --------------------------------- Macros -------------------------------- */
/* -------------------------------- Constants ------------------------------ */
SSP_DCLR_NORMAL_NODE rootCmdParser;

typedef enum Sim808ParserIndex {
    SIM_808_PARSER_A_INDEX, SIM_808_PARSER_B_INDEX,
    NUM_SIM_808_PARSER,
    SIM_808_PARSER_NONE_INDEX
} Sim808ParserIndex;

/* ------------------------------- Data types ------------------------------ */
/* -------------------------- External variables --------------------------- */
/* -------------------------- Function prototypes -------------------------- */
void sim808parser_doSearch(SSP* parser, unsigned char c);
SSP * sim808parser_getSSP(Sim808ParserIndex index);
SSP * sim808parser_initSSP(Sim808ParserIndex index, ModMgrIndex modMgrIndex, ConMgrIndex conMgrIndex);
/* -------------------- External C language linkage end -------------------- */
#ifdef __cplusplus
}
#endif

/* ------------------------------ Module end ------------------------------- */
#endif

/* ------------------------------ End of file ------------------------------ */
