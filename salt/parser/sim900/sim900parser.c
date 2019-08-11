/**
 *  \file       sim900parser.c
 *  \brief      ...
 */

/* -------------------------- Development history -------------------------- */
/*
 *  2018.05.09  LeFr  v1.0.00  Initial version
 */

/* -------------------------------- Authors -------------------------------- */
/*
 *  2018.05.09  LeFr  v1.0.00  Initial version
 *  2018.05.14  DaBa  v1.0.01  Sync response
 */

/* --------------------------------- Notes --------------------------------- */
/* ----------------------------- Include files ----------------------------- */
#include "sim900parser.h"
#include "signals.h"
#include "modmgr.h"
#include "conmgr.h"
#include "bsp.h"
#include "gps.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* ----------------------------- Local macros ------------------------------ */
/* ------------------------------- Constants ------------------------------- */
#define END_OF_RECV_STR     "\r\nOK\r\n"
#define YEAR2K              2000
#define LTBUFF_SIZE         5
#define CSQ_LENGTH   4

#define GPS_BUFF_SIZE        12
#define GPS_AUX_BUFF_SIZE   12
#define REC_CMD_LENGTH      200

/* ---------------------------- Local data types --------------------------- */
typedef struct SSP_SIM808 SSP_SIM808;
struct SSP_SIM808
{
    SSP ssp;       /* base structure */
    ModMgrIndex modMgrIndex;
    ConMgrIndex conMgrIndex;

    rui8_t isURC;

    unsigned char *prx;
    ReceivedEvt *precv;

    LocalTimeEvt localTimeEvt;
    Time *lTime;
    char ltbuf[LTBUFF_SIZE];
    char *plt;

    ImeiEvt imeiEvt;
    char *pImei;

    OperEvt copsEvt;
    char *pCops;

    char *pcsq;
    char csqBuf[CSQ_LENGTH];
    SigLevelEvt sigLevelEvt;

    GpsEvt gpsEvt;
    GpsData *gpsData;
    char gpsbuf[GPS_BUFF_SIZE];
    char gpsAuxbuf[GPS_AUX_BUFF_SIZE];
    char *pgps;

    char recCmdBuf[REC_CMD_LENGTH];
    char *recCmdNext;
};

/* ---------------------------- Global variables --------------------------- */
/* ---------------------------- Local variables ---------------------------- */
static SSP_SIM808 sim808AParser;
static SSP_SIM808 sim808BParser;
static rui8_t sim900parser;
SSP_DCLR_NORMAL_NODE at, waitOK, at_plus, at_plus_c, at_plus_ci,
                     at_plus_cip, at_plus_cips, 
                     at_plus_cipsta, at_plus_ciprxget,
                     at_plus_ciprxget_2, at_plus_ciprxget_2_wdata,
                     at_plus_cipstatus, at_plus_cipstatus_ip,
                     at_plus_cipstatus_sta, 
                     at_plus_cipstatus_c, at_plus_cipstatus_connect,
                     at_plus_cipstart,
                     at_plus_cipclose,
                     at_plus_cipsend, at_plus_cipsending, at_plus_cipsent,
                     at_plus_cpin, at_plus_creg, pinStatusHead, pinStatus, wpinSet, pinSet,
                     plus_c, plus_creg, at_plus_cifsr,
                     netClockSync,
                     at_plus_cclk, at_plus_cops, cclk_end,
                     at_plus_cgps, gps_end,
                     at_plus_gsn_pre, at_plus_creg_pre, pinStatusHead_pre;

SSP_DCLR_TRN_NODE at_plus_ciprxget_data, cclk_year, cclk_month, cclk_day,
                  cclk_hour, cclk_min, plus_csq, at_plus_gsn, cops_read,
                  gps_time, gps_status, gps_latitude, gps_latitude_indicator,
                  gps_longitude, gps_longitude_indicator, gps_speed, gps_course,
                  gps_date;

/* ----------------------- Local function prototypes ----------------------- */
static void cmd_ok(SSP_SIM808 *const me, unsigned char pos);
static void cmd_error(SSP_SIM808 *const me, unsigned char pos);
static void sim_pin(SSP_SIM808 *const me, unsigned char pos);
static void sim_error(SSP_SIM808 *const me, unsigned char pos);
static void sim_ready(SSP_SIM808 *const me, unsigned char pos);
static void isURC_set(SSP_SIM808 *const me, unsigned char pos);
static void registered(SSP_SIM808 *const me, unsigned char pos);
static void no_registered(SSP_SIM808 *const me, unsigned char pos);
static void ipInitial(SSP_SIM808 *const me, unsigned char pos);
static void ipStart(SSP_SIM808 *const me, unsigned char pos);
static void ipStatus(SSP_SIM808 *const me, unsigned char pos);
static void ipGprsAct(SSP_SIM808 *const me, unsigned char pos);
static void ipDone(SSP_SIM808 *const me, unsigned char pos);
static void connecting(SSP_SIM808 *const me, unsigned char pos);
static void closed(SSP_SIM808 *const me, unsigned char pos);
static void connected(SSP_SIM808 *const me, unsigned char pos);
static void disconnected(SSP_SIM808 *const me, unsigned char pos);
static void data_init(SSP_SIM808 *const me, unsigned char pos);
static void data_collect(SSP_SIM808 *const me, unsigned char c);
static void data_ready(SSP_SIM808 *const me, unsigned char pos);
static void netClock_rcv(SSP_SIM808 *const me, unsigned char pos);
static void lTimeInit(SSP_SIM808 *const me, unsigned char pos);
static void yearCollect(SSP_SIM808 *const me, unsigned char c);

#define monthCollect    yearCollect
#define dayCollect      yearCollect
#define hourCollect     yearCollect
#define minCollect      yearCollect

static void yearGet(SSP_SIM808 *const me, unsigned char pos);
static void monthGet(SSP_SIM808 *const me, unsigned char pos);
static void dayGet(SSP_SIM808 *const me, unsigned char pos);
static void hourGet(SSP_SIM808 *const me, unsigned char pos);
static void minGet(SSP_SIM808 *const me, unsigned char pos);
static void lTimeGet(SSP_SIM808 *const me, unsigned char pos);
static void imeiInit(SSP_SIM808 *const me, unsigned char pos);
static void imeiCollect(SSP_SIM808 *const me, unsigned char c);
static void imeiSet(SSP_SIM808 *const me, unsigned char pos);
static void copsInit(SSP_SIM808 *const me, unsigned char pos);
static void copsCollect(SSP_SIM808 *const me, unsigned char c);
static void copsSet(SSP_SIM808 *const me, unsigned char pos);
static void csqInit(SSP_SIM808 *const me, unsigned char pos);
static void csqCollect(SSP_SIM808 *const me, unsigned char c);
static void csqSet(SSP_SIM808 *const me, unsigned char pos);

static void dummy(SSP_SIM808 *const me, unsigned char pos);
static void init_end(SSP_SIM808 *const me, unsigned char pos);
static void gpsInit(SSP_SIM808 *const me, unsigned char pos);
static void gpsTimeCollect(SSP_SIM808 *const me, unsigned char c);
#define gpsStatusCollect                gpsTimeCollect
#define gpsLatitudeCollect              gpsTimeCollect
#define gpsLatitudeIndicatorCollect     gpsTimeCollect
#define gpsLongitudeCollect             gpsTimeCollect
#define gpsLongitudeIndicatorCollect    gpsTimeCollect
#define gpsSpeedCollect                 gpsTimeCollect
#define gpsCourseCollect                gpsTimeCollect
#define gpsDateCollect                  gpsTimeCollect
static void gpsTimeSet(SSP_SIM808 *const me, unsigned char pos);
static void gpsStatusSet(SSP_SIM808 *const me, unsigned char pos);
static void gpsLatitudeSet(SSP_SIM808 *const me, unsigned char pos);
static void gpsLatitudeIndicatorSet(SSP_SIM808 *const me, unsigned char pos);
static void gpsLongitudeSet(SSP_SIM808 *const me, unsigned char pos);
static void gpsLongitudeIndicatorSet(SSP_SIM808 *const me, unsigned char pos);
static void gpsSpeedSet(SSP_SIM808 *const me, unsigned char pos);
static void gpsCourseSet(SSP_SIM808 *const me, unsigned char pos);
static void gpsDateSet(SSP_SIM808 *const me, unsigned char pos);
static void gpsSet(SSP_SIM808 *const me, unsigned char pos);

void recCmdCollect(SSP_SIM808 *const me, unsigned char c);
void recCmdFlushC(SSP_SIM808 *const me, unsigned char c);
void recCmdFlush(SSP_SIM808 *const me);

/* ---------------------------- Local functions ---------------------------- */

SSP_CREATE_NORMAL_NODE(rootCmdParser);
SSP_CREATE_BR_TABLE(rootCmdParser)
	//SSPBR("STATE",      NULL,     &rootCmdParser),
	//SSPBR("CONNECT OK", NULL,     &rootCmdParser),
	SSPBR("AT",         NULL,     &at),
	SSPBR("+C",         NULL,     &plus_c),
	SSPBR("*PSUTTZ",    isURC_set,&netClockSync),
    SSPBR("SMS Ready",  init_end, &rootCmdParser),
    SSPBR("$GPRMC,",  gpsInit, &gps_time),
SSP_END_BR_TABLE

SSP_CREATE_NORMAL_NODE(at);
SSP_CREATE_BR_TABLE(at)
	SSPBR("E",  NULL,       &waitOK),
	SSPBR("+", NULL,       &at_plus),
	SSPBR("OK\r\n", cmd_ok, &rootCmdParser),
SSP_END_BR_TABLE

SSP_CREATE_NORMAL_NODE(at_plus);
SSP_CREATE_BR_TABLE(at_plus)
	SSPBR("GSN",        NULL,  &at_plus_gsn_pre),
	SSPBR("C",          NULL,      &at_plus_c),
	SSPBR("OK\r\n",     cmd_ok,    &rootCmdParser),
SSP_END_BR_TABLE

SSP_CREATE_NORMAL_NODE(at_plus_c);
SSP_CREATE_BR_TABLE(at_plus_c)
	SSPBR("PIN",                dummy,   &at_plus_cpin),
	SSPBR("REG?;+CSQ",  NULL,   &at_plus_creg_pre),
	SSPBR("STT=",           NULL,   &waitOK),
	SSPBR("I",              NULL,   &at_plus_ci),
	SSPBR("LTS=1",          NULL,   &waitOK),
	SSPBR("CLK?",           NULL,   &at_plus_cclk),
	SSPBR("OPS?",           NULL,   &at_plus_cops),
    SSPBR("MEE=1",          NULL,   &waitOK),
    SSPBR("GPS",          NULL,   &waitOK),
	SSPBR("\r\n",   NULL,  &rootCmdParser),
SSP_END_BR_TABLE

SSP_CREATE_NORMAL_NODE(at_plus_ci);
SSP_CREATE_BR_TABLE(at_plus_ci)
	SSPBR("FSR",          NULL,  &at_plus_cifsr),
	SSPBR("ICR",          NULL,  &waitOK),
	SSPBR("P",            NULL,  &at_plus_cip),
	SSPBR("\r\n",         NULL,  &rootCmdParser),
SSP_END_BR_TABLE

SSP_CREATE_NORMAL_NODE(at_plus_cip);
SSP_CREATE_BR_TABLE(at_plus_cip)
	SSPBR("S",            NULL,  &at_plus_cips),
	SSPBR("CLOSE",        NULL,  &at_plus_cipclose),
	SSPBR("RXGET",        NULL,  &at_plus_ciprxget),
	SSPBR("\r\n",         NULL,  &rootCmdParser),
SSP_END_BR_TABLE

SSP_CREATE_NORMAL_NODE(at_plus_cips);
SSP_CREATE_BR_TABLE(at_plus_cips)
	SSPBR("TA",          NULL,  &at_plus_cipsta),
	SSPBR("END",         NULL,  &at_plus_cipsend),
	SSPBR("HUT",         NULL,  &waitOK),
	SSPBR("\r\n",        NULL,  &rootCmdParser),
SSP_END_BR_TABLE

SSP_CREATE_NORMAL_NODE(at_plus_cipsta);
SSP_CREATE_BR_TABLE(at_plus_cipsta)
	SSPBR("TUS;+CSQ",     NULL,  &at_plus_cipstatus),
	SSPBR("RT=",          NULL,  &at_plus_cipstart),
	SSPBR("\r\n",         NULL,  &rootCmdParser),
SSP_END_BR_TABLE

/* --------------------------------------------------------------- */
/* ---------------------------- AT+CPIN --------------------------- */
SSP_CREATE_NORMAL_NODE(at_plus_cpin);
SSP_CREATE_BR_TABLE(at_plus_cpin)
	SSPBR("?",            NULL,  &pinStatusHead_pre),
	SSPBR("=",            NULL,  &wpinSet),
	SSPBR("\r\n",         NULL,  &rootCmdParser),
SSP_END_BR_TABLE

SSP_CREATE_NORMAL_NODE(pinStatusHead_pre);
SSP_CREATE_BR_TABLE(pinStatusHead_pre)
                SSPBR("\r\n",    NULL,&pinStatusHead),
SSP_END_BR_TABLE


SSP_CREATE_NORMAL_NODE(pinStatusHead);
SSP_CREATE_BR_TABLE(pinStatusHead)
	SSPBR("+CPIN: ", NULL,&pinStatus),
	SSPBR("\r\n",    NULL,&rootCmdParser),
SSP_END_BR_TABLE

SSP_CREATE_NORMAL_NODE(pinStatus);
SSP_CREATE_BR_TABLE(pinStatus)
                SSPBR("SIM PIN", sim_pin,   &rootCmdParser),
                SSPBR("ERROR",   sim_error, &rootCmdParser),
                SSPBR("READY",   sim_ready, &rootCmdParser),
                SSPBR("\r\n",    NULL,      &rootCmdParser),
SSP_END_BR_TABLE

SSP_CREATE_NORMAL_NODE(wpinSet);
SSP_CREATE_BR_TABLE(wpinSet)
	SSPBR("\r\n\r\n",   NULL,   &pinSet),
SSP_END_BR_TABLE


SSP_CREATE_NORMAL_NODE(pinSet);
SSP_CREATE_BR_TABLE(pinSet)
	SSPBR("OK\r\n", cmd_ok, &rootCmdParser),
	SSPBR("\r\n",   NULL,   &rootCmdParser),
SSP_END_BR_TABLE

/* --------------------------------------------------------------- */
/* --------------------------- AT+CREG --------------------------- */
SSP_CREATE_NORMAL_NODE(at_plus_creg_pre);
SSP_CREATE_BR_TABLE(at_plus_creg_pre)
                SSPBR("\r\n",    NULL,  &at_plus_creg),
SSP_END_BR_TABLE

SSP_CREATE_NORMAL_NODE(at_plus_creg);
SSP_CREATE_BR_TABLE(at_plus_creg)
	SSPBR("1,",      NULL,  &plus_creg),
	SSPBR("\r\n",    NULL,  &rootCmdParser),
SSP_END_BR_TABLE

/* --------------------------------------------------------------- */
/* --------------------------- AT+CCCLK --------------------------- */
SSP_CREATE_NORMAL_NODE(at_plus_cclk);
SSP_CREATE_BR_TABLE(at_plus_cclk)
	SSPBR("+CCLK: \"",  lTimeInit,  &cclk_year),
SSP_END_BR_TABLE

SSP_CREATE_TRN_NODE(cclk_year, yearCollect);
SSP_CREATE_BR_TABLE(cclk_year)
	SSPBR("/",      yearGet,  &cclk_month),
SSP_END_BR_TABLE

SSP_CREATE_TRN_NODE(cclk_month, monthCollect);
SSP_CREATE_BR_TABLE(cclk_month)
	SSPBR("/",      monthGet,  &cclk_day),
SSP_END_BR_TABLE

SSP_CREATE_TRN_NODE(cclk_day, dayCollect);
SSP_CREATE_BR_TABLE(cclk_day)
	SSPBR(",",      dayGet,  &cclk_hour),
SSP_END_BR_TABLE

SSP_CREATE_TRN_NODE(cclk_hour, hourCollect);
SSP_CREATE_BR_TABLE(cclk_hour)
	SSPBR(":",      hourGet,  &cclk_min),
SSP_END_BR_TABLE

SSP_CREATE_TRN_NODE(cclk_min, minCollect);
SSP_CREATE_BR_TABLE(cclk_min)
	SSPBR(":",      minGet,  &cclk_end),
SSP_END_BR_TABLE

SSP_CREATE_NORMAL_NODE(cclk_end);
SSP_CREATE_BR_TABLE(cclk_end)
	SSPBR("OK\r\n", lTimeGet,  &rootCmdParser),
SSP_END_BR_TABLE

/* --------------------------------------------------------------- */
/* --------------------------- AT+COPS?--------------------------- */
SSP_CREATE_NORMAL_NODE(at_plus_cops);
SSP_CREATE_BR_TABLE(at_plus_cops)
	SSPBR("\"",     copsInit,  &cops_read),
	SSPBR("OK\r\n", NULL,    &rootCmdParser),
SSP_END_BR_TABLE

SSP_CREATE_TRN_NODE(cops_read, copsCollect);
SSP_CREATE_BR_TABLE(cops_read)
	SSPBR("\"",   copsSet,  &rootCmdParser),
	SSPBR("\r\n", NULL,    &rootCmdParser),
SSP_END_BR_TABLE

/* --------------------------------------------------------------- */

/* --------------------------------------------------------------- */
/* ------------------------ AT+CIPRXGET -------------------------- */
SSP_CREATE_NORMAL_NODE(at_plus_ciprxget);
SSP_CREATE_BR_TABLE(at_plus_ciprxget)
	SSPBR("1",               NULL,    &waitOK),
	SSPBR("2,",              NULL,    &at_plus_ciprxget_2),
	SSPBR("\r\n",            NULL,    &rootCmdParser),
SSP_END_BR_TABLE

/* --------------------------------------------------------------- */
/* ------------------------ AT+CIPRXGET=2 ------------------------ */
SSP_CREATE_NORMAL_NODE(at_plus_ciprxget_2);
SSP_CREATE_BR_TABLE(at_plus_ciprxget_2)
	SSPBR("ERROR",             cmd_error, &rootCmdParser),
	SSPBR("+CIPRXGET: 2",      NULL,   &at_plus_ciprxget_2_wdata),
SSP_END_BR_TABLE

SSP_CREATE_NORMAL_NODE(at_plus_ciprxget_2_wdata);
SSP_CREATE_BR_TABLE(at_plus_ciprxget_2_wdata)
	SSPBR("\r\n",      data_init,   &at_plus_ciprxget_data),
SSP_END_BR_TABLE

SSP_CREATE_TRN_NODE(at_plus_ciprxget_data, data_collect);
SSP_CREATE_BR_TABLE(at_plus_ciprxget_data)
	SSPBR(END_OF_RECV_STR, data_ready,   &rootCmdParser),
SSP_END_BR_TABLE

/* --------------------------------------------------------------- */
/* ------------------------ AT+CIPSTART -------------------------- */
SSP_CREATE_NORMAL_NODE(at_plus_cipstart);
SSP_CREATE_BR_TABLE(at_plus_cipstart)
	SSPBR("OK",     cmd_ok,    &rootCmdParser),
	SSPBR("ERROR",  cmd_error, &rootCmdParser),
SSP_END_BR_TABLE

/* --------------------------------------------------------------- */
/* ------------------------ AT+CIPSTATUS ------------------------- */
SSP_CREATE_NORMAL_NODE(at_plus_cipstatus);
SSP_CREATE_BR_TABLE(at_plus_cipstatus)
	SSPBR("IP ",    NULL,  &at_plus_cipstatus_ip),
	SSPBR("C",      NULL,  &at_plus_cipstatus_c),
SSP_END_BR_TABLE

SSP_CREATE_NORMAL_NODE(at_plus_cipstatus_ip);
SSP_CREATE_BR_TABLE(at_plus_cipstatus_ip)
	SSPBR("INITIAL",    ipInitial,  &rootCmdParser),
	SSPBR("STA",        NULL,       &at_plus_cipstatus_sta),
	SSPBR("GPRSACT",    ipGprsAct,  &rootCmdParser),
SSP_END_BR_TABLE

SSP_CREATE_NORMAL_NODE(at_plus_cipstatus_sta);
SSP_CREATE_BR_TABLE(at_plus_cipstatus_sta)
	SSPBR("TUS",     ipStatus,   &rootCmdParser),
	SSPBR("RT",      ipStart,    &rootCmdParser),
SSP_END_BR_TABLE

SSP_CREATE_NORMAL_NODE(at_plus_cipstatus_c);
SSP_CREATE_BR_TABLE(at_plus_cipstatus_c)
	SSPBR("LOSED",     closed,     &rootCmdParser),
	SSPBR("ONNECT",    NULL,       &at_plus_cipstatus_connect),
SSP_END_BR_TABLE

SSP_CREATE_NORMAL_NODE(at_plus_cipstatus_connect);
SSP_CREATE_BR_TABLE(at_plus_cipstatus_connect)
	SSPBR("ING",     connecting,     &rootCmdParser),
	SSPBR(" OK",     connected,      &rootCmdParser),
SSP_END_BR_TABLE

/* --------------------------------------------------------------- */
/* ------------------------- AT+CIPSEND -------------------------- */
SSP_CREATE_NORMAL_NODE(at_plus_cipsend);
SSP_CREATE_BR_TABLE(at_plus_cipsend)
	SSPBR("ERROR",  cmd_error, &rootCmdParser),
#ifdef _SEND_WITH_TERMINATOR
	SSPBR(">", cmd_ok,  &at_plus_cipsending),
#else
	SSPBR(">", cmd_ok,  &at_plus_cipsent),
#endif
SSP_END_BR_TABLE

#ifdef _SEND_WITH_TERMINATOR
SSP_CREATE_NORMAL_NODE(at_plus_cipsending);
SSP_CREATE_BR_TABLE(at_plus_cipsending)
	SSPBR("\x1A", NULL,  &at_plus_cipsent),
SSP_END_BR_TABLE
#endif

SSP_CREATE_NORMAL_NODE(at_plus_cipsent);
SSP_CREATE_BR_TABLE(at_plus_cipsent)
#ifdef GPRS_QUICK_SEND
	SSPBR("DATA ACCEPT", cmd_ok,  &rootCmdParser),
#else
	SSPBR("SEND OK\r\n", cmd_ok,  &rootCmdParser),
#endif
SSP_END_BR_TABLE

/* --------------------------------------------------------------- */
/* ------------------------- AT+CIPCLOSE ------------------------- */
SSP_CREATE_NORMAL_NODE(at_plus_cipclose);
SSP_CREATE_BR_TABLE(at_plus_cipclose)
	SSPBR("CLOSE OK", disconnected,  &rootCmdParser),
SSP_END_BR_TABLE

/* --------------------------------------------------------------- */
/* -------------------------- AT+CIFSR --------------------------- */
SSP_CREATE_NORMAL_NODE(at_plus_cifsr);
SSP_CREATE_BR_TABLE(at_plus_cifsr)
	SSPBR(".",       ipDone,      &rootCmdParser),
SSP_END_BR_TABLE

/* --------------------------------------------------------------- */
/* --------------------------- initStr --------------------------- */
/* --------------------------- AT+CSTT --------------------------- */
/* --------------------------- AT+CIICT -------------------------- */
SSP_CREATE_NORMAL_NODE(waitOK);
SSP_CREATE_BR_TABLE(waitOK)
	SSPBR("OK\r\n",     cmd_ok, &rootCmdParser),
	SSPBR("ERROR\r\n",  NULL,   &rootCmdParser),
SSP_END_BR_TABLE

/* --------------------------------------------------------------- */
SSP_CREATE_NORMAL_NODE(plus_c);
SSP_CREATE_BR_TABLE(plus_c)
	SSPBR("REG:",   isURC_set, &plus_creg),
	SSPBR("SQ: ",  csqInit,   &plus_csq),
    SSPBR("PIN",   recCmdFlushC,      &rootCmdParser),
	SSPBR("\r\n",   NULL,      &rootCmdParser),
SSP_END_BR_TABLE

/* -------------------- Unsolicited +CREG ... -------------------- */
SSP_CREATE_NORMAL_NODE(plus_creg);
SSP_CREATE_BR_TABLE(plus_creg)
	SSPBR("0",     no_registered, &rootCmdParser),
	SSPBR("1",     registered,    &rootCmdParser),
	SSPBR("2",     no_registered, &rootCmdParser),
	SSPBR("3",     no_registered, &rootCmdParser),
	SSPBR("4",     no_registered, &rootCmdParser),
	SSPBR("5",     registered,    &rootCmdParser),
	SSPBR("\r\n",  NULL,          &rootCmdParser),
SSP_END_BR_TABLE

/* -------------------- Unsolicited +CSQ  ... -------------------- */
SSP_CREATE_TRN_NODE(plus_csq, csqCollect);
SSP_CREATE_BR_TABLE(plus_csq)
	SSPBR(",", csqSet, &rootCmdParser),
SSP_END_BR_TABLE

/* --------------------------------------------------------------- */
/* -------------------- Unsolicited *PSUTTZ... ------------------- */
SSP_CREATE_NORMAL_NODE(netClockSync);
SSP_CREATE_BR_TABLE(netClockSync)
	SSPBR("\r\n",   netClock_rcv, &rootCmdParser),
SSP_END_BR_TABLE

/* ---------------------------- AT+GSN --------------------------- */
SSP_CREATE_NORMAL_NODE(at_plus_gsn_pre);
SSP_CREATE_BR_TABLE(at_plus_gsn_pre)
                SSPBR("\r\n",   imeiInit, &at_plus_gsn),
SSP_END_BR_TABLE


SSP_CREATE_TRN_NODE(at_plus_gsn, imeiCollect);
SSP_CREATE_BR_TABLE(at_plus_gsn)
	SSPBR("OK\r\n", imeiSet, &rootCmdParser),
SSP_END_BR_TABLE

/* ---------------------------- AT+CGPS --------------------------- */

SSP_CREATE_NORMAL_NODE(at_plus_cgps);
SSP_CREATE_BR_TABLE(at_plus_cgps)
                SSPBR("PWR=1;+CGPSOUT=32",  NULL, &waitOK),
                SSPBR("OUT=0;+CGPSPWR=0",   NULL, &waitOK),
                SSPBR("\r\n",   NULL, &rootCmdParser),
SSP_END_BR_TABLE

SSP_CREATE_TRN_NODE(gps_time, gpsTimeCollect);
SSP_CREATE_BR_TABLE(gps_time)
                SSPBR(",",      gpsTimeSet,  &gps_status),
SSP_END_BR_TABLE

SSP_CREATE_TRN_NODE(gps_status, gpsStatusCollect);
SSP_CREATE_BR_TABLE(gps_status)
                SSPBR(",",      gpsStatusSet,  &gps_latitude),
SSP_END_BR_TABLE

SSP_CREATE_TRN_NODE(gps_latitude, gpsLatitudeCollect);
SSP_CREATE_BR_TABLE(gps_latitude)
                SSPBR(",",      gpsLatitudeSet,  &gps_latitude_indicator),
SSP_END_BR_TABLE

SSP_CREATE_TRN_NODE(gps_latitude_indicator, gpsLatitudeIndicatorCollect);
SSP_CREATE_BR_TABLE(gps_latitude_indicator)
                SSPBR(",",      gpsLatitudeIndicatorSet,  &gps_longitude),
SSP_END_BR_TABLE

SSP_CREATE_TRN_NODE(gps_longitude, gpsLongitudeCollect);
SSP_CREATE_BR_TABLE(gps_longitude)
                SSPBR(",",      gpsLongitudeSet,  &gps_longitude_indicator),
SSP_END_BR_TABLE

SSP_CREATE_TRN_NODE(gps_longitude_indicator, gpsLongitudeIndicatorCollect);
SSP_CREATE_BR_TABLE(gps_longitude_indicator)
                SSPBR(",",      gpsLongitudeIndicatorSet,  &gps_speed),
SSP_END_BR_TABLE

SSP_CREATE_TRN_NODE(gps_speed, gpsSpeedCollect);
SSP_CREATE_BR_TABLE(gps_speed)
                SSPBR(",",      gpsSpeedSet,  &gps_course),
SSP_END_BR_TABLE

SSP_CREATE_TRN_NODE(gps_course, gpsCourseCollect);
SSP_CREATE_BR_TABLE(gps_course)
                SSPBR(",",      gpsCourseSet,  &gps_date),
SSP_END_BR_TABLE

SSP_CREATE_TRN_NODE(gps_date, gpsDateCollect);
SSP_CREATE_BR_TABLE(gps_date)
                SSPBR(",",      gpsDateSet,  &gps_end),
SSP_END_BR_TABLE

SSP_CREATE_NORMAL_NODE(gps_end);
SSP_CREATE_BR_TABLE(gps_end)
                SSPBR("\n",      gpsSet,  &rootCmdParser),
SSP_END_BR_TABLE

/* --------------------------------------------------------------- */

static void
sendModResp_noArgs(SSP_SIM808 *const me, RKH_SIG_T sig)
{
    ModMgrResp *p;
    RKH_SIG_T ModMgrSignal = evResponse;

    recCmdFlush(me);
       
    if(me->isURC)
    {
        me->isURC = 0;
        ModMgrSignal = evURC;
    }

    p = RKH_ALLOC_EVT( ModMgrResp, ModMgrSignal, &sim900parser );
    p->fwdEvt = sig;
    RKH_SMA_POST_FIFO(modMgr_GetModMgr(me->modMgrIndex), RKH_UPCAST(RKH_EVT_T, p), &sim900parser );
}

static void
isURC_set(SSP_SIM808 *const me, unsigned char pos)
{
    (void)pos;

    me->isURC = 1;
}

static void
cmd_ok(SSP_SIM808 *const me, unsigned char pos)
{
    (void)pos;
    
    sendModResp_noArgs(me, evOk);
}

static void
cmd_error(SSP_SIM808 *const me, unsigned char pos)
{

    
    sendModResp_noArgs(me, evError);
}

static void
sim_pin(SSP_SIM808 *const me, unsigned char pos)
{
    (void)pos;
    
    sendModResp_noArgs(me, evSimPin);
}

static void
sim_error(SSP_SIM808 *const me, unsigned char pos)
{
    (void)pos;
    
    sendModResp_noArgs(me, evSimError);
}

static void
sim_ready(SSP_SIM808 *const me, unsigned char pos)
{
    (void)pos;
    
    sendModResp_noArgs(me, evSimReady);
}

static void
registered(SSP_SIM808 *const me, unsigned char pos)
{
    (void)pos;
    
    sendModResp_noArgs(me, evReg);
}

static void
no_registered(SSP_SIM808 *const me, unsigned char pos)
{
    (void)pos;
    
    sendModResp_noArgs(me, evNoReg);
}

static void
ipInitial(SSP_SIM808 *const me, unsigned char pos)
{
    (void)pos;
    
    sendModResp_noArgs(me, evIPInitial);
}

static void
ipStart(SSP_SIM808 *const me, unsigned char pos)
{
    (void)pos;
    
    sendModResp_noArgs(me, evIPStart);
}

static void
ipStatus(SSP_SIM808 *const me, unsigned char pos)
{
    (void)pos;
    
    sendModResp_noArgs(me, evIPStatus);
}

static void
ipGprsAct(SSP_SIM808 *const me, unsigned char pos)
{
    (void)pos;
    
    sendModResp_noArgs(me, evIPGprsAct);
}

static void
ipDone(SSP_SIM808 *const me, unsigned char pos)
{
    (void)pos;

    sendModResp_noArgs(me, evIP);
}

static void
connecting(SSP_SIM808 *const me, unsigned char pos)
{
    (void)pos;

    sendModResp_noArgs(me, evConnecting);
}

static void
closed(SSP_SIM808 *const me, unsigned char pos)
{
    (void)pos;

    sendModResp_noArgs(me, evClosed);
}

static void
connected(SSP_SIM808 *const me, unsigned char pos)
{
    (void)pos;

    sendModResp_noArgs(me, evConnected);
}

static void
disconnected(SSP_SIM808 *const me, unsigned char pos)
{
    (void)pos;

    sendModResp_noArgs(me, evDisconnected);
}

void
data_init(SSP_SIM808 *const me, unsigned char c)
{
    (void)c;

    me->precv = ConMgr_ReceiveDataGetRef(me->conMgrIndex);
    me->precv->size = 0;
    me->prx = me->precv->buf;
}

static void
data_collect(SSP_SIM808 *const me, unsigned char c)
{
    *(me->prx) = c;
    ++(me->prx);
    ++(me->precv->size);
}

static void
data_ready(SSP_SIM808 *const me, unsigned char pos)
{
    (void)pos;
    
    *(me->prx) = '\0';
    me->precv->size -= (sizeof(END_OF_RECV_STR) - 1);
   
    sendModResp_noArgs(me, evOk);
}

static void
netClock_rcv(SSP_SIM808 *const me, unsigned char pos)
{
    (void)pos;
    
    sendModResp_noArgs(me, evNetClockSync);
}

static void
lTimeInit(SSP_SIM808 *const me, unsigned char pos)
{
	(void)pos;

    me->plt = me->ltbuf;
    *(me->plt) = '\0';

    me->lTime = &(me->localTimeEvt.time);
}

static void
yearCollect(SSP_SIM808 *const me, unsigned char c)
{
    if(me->plt >= me->ltbuf + LTBUFF_SIZE)
        return;

    *(me->plt) = c;
    ++(me->plt);
}

static void
yearGet(SSP_SIM808 *const me, unsigned char pos)
{
	(void)pos;

    *(me->plt) = '\0';

    me->lTime->tm_year = (short)(YEAR2K + atoi(me->ltbuf));

    me->plt = me->ltbuf;
}

static void
monthGet(SSP_SIM808 *const me, unsigned char pos)
{
	(void)pos;

    *(me->plt) = '\0';

    me->lTime->tm_mon = (unsigned char)atoi(me->ltbuf);

    me->plt = me->ltbuf;
}

static void
dayGet(SSP_SIM808 *const me, unsigned char pos)
{
	(void)pos;

    *(me->plt) = '\0';

    me->lTime->tm_mday = (unsigned char)atoi(me->ltbuf);

    me->plt = me->ltbuf;
}

static void
hourGet(SSP_SIM808 *const me, unsigned char pos)
{
	(void)pos;

    *(me->plt) = '\0';

    me->lTime->tm_hour = (unsigned char)atoi(me->ltbuf);

    me->plt = me->ltbuf;
}

static void
minGet(SSP_SIM808 *const me, unsigned char pos)
{
	(void)pos;

    *(me->plt) = '\0';

    me->lTime->tm_min = (unsigned char)atoi(me->ltbuf);

    me->plt = me->ltbuf;
}

static void
lTimeGet(SSP_SIM808 *const me, unsigned char pos)
{
	(void)pos;

    recCmdFlush(me);

    me->lTime->tm_sec = 30;

    RKH_SET_STATIC_EVENT(RKH_UPCAST(RKH_EVT_T, &(me->localTimeEvt)), evResponse);

    me->localTimeEvt.e.fwdEvt = evLocalTime;
        
    RKH_SMA_POST_FIFO(modMgr_GetModMgr(me->modMgrIndex), RKH_UPCAST(RKH_EVT_T, &(me->localTimeEvt)),
						      &sim900parser);

}

static void
imeiInit(SSP_SIM808 *const me, unsigned char pos)
{
    (void)pos;

    me->pImei = me->imeiEvt.buf;
}

static void
imeiCollect(SSP_SIM808 *const me, unsigned char c)
{
    if(me->pImei >= (me->imeiEvt.buf + sizeof(me->imeiEvt.buf) - 1))
        return;
    
    *(me->pImei) = c;
    ++(me->pImei);
}

static void
imeiSet(SSP_SIM808 *const me, unsigned char pos)
{
	(void)pos;

    recCmdFlush(me);

    me->imeiEvt.buf[IMEI_LENGTH] = '\0';

    RKH_SET_STATIC_EVENT(RKH_UPCAST(RKH_EVT_T, &(me->imeiEvt)), evResponse);

    me->imeiEvt.e.fwdEvt = evImei;
        
    RKH_SMA_POST_FIFO(modMgr_GetModMgr(me->modMgrIndex), RKH_UPCAST(RKH_EVT_T, &(me->imeiEvt)),
						      &sim900parser);
}

static void
copsInit(SSP_SIM808 *const me, unsigned char pos)
{
    (void)pos;

    me->pCops = me->copsEvt.buf;
}

static void
copsCollect(SSP_SIM808 *const me, unsigned char c)
{
    if(me->pCops >= (me->copsEvt.buf + sizeof(me->copsEvt.buf) - 1))
        return;
    
    *(me->pCops) = c;
    ++(me->pCops);
}

static void
copsSet(SSP_SIM808 *const me, unsigned char pos)
{
	(void)pos;

    recCmdFlush(me);

    *(me->pCops - 1) = '\0';

    RKH_SET_STATIC_EVENT(RKH_UPCAST(RKH_EVT_T, &(me->copsEvt)), evResponse);

    me->copsEvt.e.fwdEvt = evOper;
        
    RKH_SMA_POST_FIFO(modMgr_GetModMgr(me->modMgrIndex), RKH_UPCAST(RKH_EVT_T, &(me->copsEvt)),
						      &sim900parser);
}

static void
csqInit(SSP_SIM808 *const me, unsigned char pos)
{
    (void)pos;

    me->pcsq = me->csqBuf;
}

static void
csqCollect(SSP_SIM808 *const me, unsigned char c)
{
    if(me->pcsq >= (me->csqBuf + sizeof(me->csqBuf) - 1))
        return;

    *(me->pcsq) = c;
    ++(me->pcsq);
}

static void
csqSet(SSP_SIM808 *const me, unsigned char pos)
{
    (void)pos;

    recCmdFlush(me);

    *(me->pcsq) = '\0';
    me->sigLevelEvt.value = atoi(me->csqBuf);

    RKH_SET_STATIC_EVENT(RKH_UPCAST(RKH_EVT_T, &(me->sigLevelEvt)), evURC);

    me->sigLevelEvt.e.fwdEvt = evSigLevel;
        
    RKH_SMA_POST_FIFO(modMgr_GetModMgr(me->modMgrIndex), RKH_UPCAST(RKH_EVT_T, &(me->sigLevelEvt)),
						      &sim900parser);
}

static void dummy(SSP_SIM808 *const me, unsigned char pos){
    int i = 0;
}

static void init_end(SSP_SIM808 *const me, unsigned char pos){
    me->isURC = 1;

    sendModResp_noArgs(me, evInitEnd);
}

static void gpsInit(SSP_SIM808 *const me, unsigned char pos){
    (void)pos;

    me->pgps = me->gpsbuf;
    *(me->pgps) = '\0';

    me->gpsData = &(me->gpsEvt.gpsData);
}
static void gpsTimeCollect(SSP_SIM808 *const me, unsigned char c){
    if(me->pgps >= me->gpsbuf + GPS_BUFF_SIZE)
        return;

    *(me->pgps) = c;
    ++(me->pgps);
}
static void gpsTimeSet(SSP_SIM808 *const me, unsigned char pos){

    (void)pos;

    *(me->pgps) = '\0';

    memcpy(me->gpsAuxbuf, me->gpsbuf, 2);
    me->gpsAuxbuf[2]='\0';
    me->gpsData->time.tm_hour = (unsigned char)atoi(me->gpsAuxbuf);

    memcpy(me->gpsAuxbuf, me->gpsbuf + 2, 2);
    me->gpsAuxbuf[2]='\0';
    me->gpsData->time.tm_min = (unsigned char)atoi(me->gpsAuxbuf);

    memcpy(me->gpsAuxbuf, me->gpsbuf + 4, 2);
    me->gpsAuxbuf[2]='\0';
    me->gpsData->time.tm_sec = (unsigned char)atoi(me->gpsAuxbuf);

    me->pgps = me->gpsbuf;
}
static void gpsStatusSet(SSP_SIM808 *const me, unsigned char pos){
    (void)pos;

    *(me->pgps) = '\0';

    switch (me->gpsbuf[0]){
        case 'A':
            me->gpsData->valid = RKH_TRUE;
            break;
        case 'V':
        default:
            me->gpsData->valid = RKH_FALSE;
            break;
    }

    me->pgps = me->gpsbuf;
}
static void gpsLatitudeSet(SSP_SIM808 *const me, unsigned char pos){
    (void)pos;

    *(me->pgps) = '\0';

    memcpy(me->gpsAuxbuf, me->gpsbuf, 2);
    me->gpsAuxbuf[2]='\0';
    me->gpsData->latitude_deg = (unsigned char)atoi(me->gpsAuxbuf);

    strcpy(me->gpsAuxbuf, me->gpsbuf + 2);
    me->gpsData->latitude_min = (float)atof(me->gpsAuxbuf);

    me->pgps = me->gpsbuf;
}
static void gpsLatitudeIndicatorSet(SSP_SIM808 *const me, unsigned char pos){
    (void)pos;

    *(me->pgps) = '\0';

    switch (me->gpsbuf[0]){
        case 'N':
            me->gpsData->latitude_north = RKH_TRUE;
            break;
        case 'S':
        default:
            me->gpsData->latitude_north = RKH_FALSE;
            break;
    }

    me->pgps = me->gpsbuf;
}
static void gpsLongitudeSet(SSP_SIM808 *const me, unsigned char pos){
    (void)pos;

    *(me->pgps) = '\0';

    memcpy(me->gpsAuxbuf, me->gpsbuf, 2);
    me->gpsAuxbuf[2]='\0';
    me->gpsData->longitude_deg = (unsigned char)atoi(me->gpsAuxbuf);

    strcpy(me->gpsAuxbuf, me->gpsbuf + 2);
    me->gpsData->longitude_min = (float)atof(me->gpsAuxbuf);

    me->pgps = me->gpsbuf;
}
static void gpsLongitudeIndicatorSet(SSP_SIM808 *const me, unsigned char pos){
    (void)pos;

    *(me->pgps) = '\0';

    switch (me->gpsbuf[0]){
        case 'E':
            me->gpsData->longitude_east = RKH_TRUE;
            break;
        case 'W':
        default:
            me->gpsData->longitude_east = RKH_FALSE;
            break;
    }

    me->pgps = me->gpsbuf;
}
static void gpsSpeedSet(SSP_SIM808 *const me, unsigned char pos){
    (void)pos;

    *(me->pgps) = '\0';

    me->gpsData->speed = (float)atof(me->gpsbuf);

    me->pgps = me->gpsbuf;
}
static void gpsCourseSet(SSP_SIM808 *const me, unsigned char pos){
    (void)pos;

    *(me->pgps) = '\0';

    me->gpsData->course = (float)atof(me->gpsbuf);

    me->pgps = me->gpsbuf;
}
static void gpsDateSet(SSP_SIM808 *const me, unsigned char pos){
    (void)pos;

    *(me->pgps) = '\0';

    memcpy(me->gpsAuxbuf, me->gpsbuf, 2);
    me->gpsAuxbuf[2]='\0';
    me->gpsData->time.tm_mday = (unsigned char)atoi(me->gpsAuxbuf);

    memcpy(me->gpsAuxbuf, me->gpsbuf + 2, 2);
    me->gpsAuxbuf[2]='\0';
    me->gpsData->time.tm_mon = (unsigned char)atoi(me->gpsAuxbuf);

    memcpy(me->gpsAuxbuf, me->gpsbuf + 4, 2);
    me->gpsAuxbuf[2]='\0';
    me->gpsData->time.tm_year = (short)(YEAR2K + atoi(me->gpsAuxbuf));

    me->pgps = me->gpsbuf;
}
static void gpsSet(SSP_SIM808 *const me, unsigned char pos)
{
    (void)pos;

    recCmdFlush(me);

    RKH_SET_STATIC_EVENT(RKH_UPCAST(RKH_EVT_T, &(me->gpsEvt)), evURC);

    me->gpsEvt.e.fwdEvt = evGps;

    RKH_SMA_POST_FIFO(modMgr_GetModMgr(me->modMgrIndex), RKH_UPCAST(RKH_EVT_T, &(me->gpsEvt)),
                      &sim900parser);
}

void recCmdCollect(SSP_SIM808 *const me, unsigned char c){
    if(me->recCmdNext == NULL){
        me->recCmdNext = me->recCmdBuf;
    }
    if(me->recCmdNext >= (me->recCmdBuf + sizeof(me->recCmdBuf) - 1))
        return;

    *(me->recCmdNext) = c;
    ++(me->recCmdNext);
}

void recCmdFlushC(SSP_SIM808 *const me, unsigned char c){
    recCmdFlush(me);
}

void recCmdFlush(SSP_SIM808 *const me){

    *(me->recCmdNext) = '\0';

    char * a = me->recCmdBuf

    RKH_TRC_USR_BEGIN(USR_TRACE_IN)
        RKH_TUSR_STR(me->recCmdBuf);
    RKH_TRC_USR_END();

    me->recCmdNext = me->recCmdBuf;

}

/* ---------------------------- Global functions --------------------------- */
void sim808parser_doSearch(SSP* parser, unsigned char c)
{
    recCmdCollect(RKH_DOWNCAST(SSP_SIM808,parser), c);
    ssp_doSearch(parser, c);
}
SSP * sim808parser_getSSP(Sim808ParserIndex index){
    switch (index){
        case SIM_808_PARSER_A_INDEX:
            return RKH_UPCAST(SSP,&sim808AParser);
        case SIM_808_PARSER_B_INDEX:
            return RKH_UPCAST(SSP,&sim808BParser);
        default:
            break;
    }
    return NULL;
}
SSP * sim808parser_initSSP(Sim808ParserIndex index, ModMgrIndex modMgrIndex, ConMgrIndex conMgrIndex){
    SSP_SIM808 * result = NULL;
    switch (index){
        case SIM_808_PARSER_A_INDEX:
            result = &sim808AParser;
            break;
        case SIM_808_PARSER_B_INDEX:
            result = &sim808BParser;
            break;
        default:
            break;
    }
    if(result != NULL){
        result->modMgrIndex = modMgrIndex;
        result->conMgrIndex = conMgrIndex;
    }
    return RKH_UPCAST(SSP,result);
}

/* ------------------------------ End of file ------------------------------ */
