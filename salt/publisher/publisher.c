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

/* ----------------------------- Local macros ------------------------------ */
/* ------------------------------- Constants ------------------------------- */
/* ---------------------------- Local data types --------------------------- */
/* ---------------------------- Global variables --------------------------- */
/* ---------------------------- Local variables ---------------------------- */
char dataBuf[1024];

/* ----------------------- Local function prototypes ----------------------- */
/* ---------------------------- Local functions ---------------------------- */
static rui16_t
getNextPublishTime()
{
    return 8;
}

/* ---------------------------- Global functions --------------------------- */
rui16_t
publishDimba(AppData *appMsg)
{
    jwOpen( dataBuf, sizeof(dataBuf), JW_OBJECT, JW_COMPACT );
    jwObj_int("id", atoi(ConMgr_imeiSNR()));
    jwClose();

    appMsg->data = (rui8_t *)dataBuf;
    appMsg->size = (rui16_t)strlen(dataBuf);
    return getNextPublishTime();
}

/* ------------------------------ End of file ------------------------------ */
