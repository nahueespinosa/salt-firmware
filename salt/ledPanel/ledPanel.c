/**
 *  \file       ledPanel.c
 *  \brief      AS1116 driver.
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
#include "sapi.h"
#include "ledPanel.h"
#include "sapi_spi.h"
#include "as1116.h"

/* ----------------------------- Local macros ------------------------------ */
/* ------------------------------- Constants ------------------------------- */

const rui8_t * const numeralCodes = digitCodeMap;
const rui8_t * const alphaCodes = digitCodeMap + 10;

/* ---------------------------- Local data types --------------------------- */
/* ---------------------------- Global variables --------------------------- */
/* ---------------------------- Local variables ---------------------------- */
rui8_t digits[NUM_DIGITS];
/* ----------------------- Local function prototypes ----------------------- */

void ledPanelSetCfgInt(LedPanelCfg* cfg);
/* ---------------------------- Local functions ---------------------------- */

void ledPanelSetCfgInt(LedPanelCfg* cfg){


    as1116SetDigit(DIGIT_0_POS, cfg->digit0);
    as1116SetDigit(DIGIT_1_POS, cfg->digit1);
    as1116SetDigit(DIGIT_2_POS, cfg->digit2);
    as1116SetDigit(DIGIT_3_POS, cfg->digit3);

    rui8_t aux = 0x00;
    aux |= ((cfg->ledOn & LED_R_MASK) >> LED_R_POS) << LED_ON_SEGMENT_POS;
    aux |= ((cfg->ledCt & LED_R_MASK) >> LED_R_POS) << LED_CT_SEGMENT_POS;
    aux |= ((cfg->ledFe & LED_R_MASK) >> LED_R_POS) << LED_FE_SEGMENT_POS;
    aux |= ((cfg->ledGps & LED_R_MASK) >> LED_R_POS) << LED_GPS_SEGMENT_POS;
    aux |= ((cfg->ledRemoteOp & LED_R_MASK) >> LED_R_POS) << LED_REMOTE_OP_SEGMENT_POS;
    aux |= ((cfg->ledIsolated & LED_R_MASK) >> LED_R_POS) << LED_ISOLATED_SEGMENT_POS;
    as1116SetDigit(DIGIT_RED_POS, aux);

    aux = 0x00;
    aux |= ((cfg->ledOn & LED_G_MASK) >> LED_G_POS) << LED_ON_SEGMENT_POS;
    aux |= ((cfg->ledCt & LED_G_MASK) >> LED_G_POS) << LED_CT_SEGMENT_POS;
    aux |= ((cfg->ledFe & LED_G_MASK) >> LED_G_POS) << LED_FE_SEGMENT_POS;
    aux |= ((cfg->ledGps & LED_G_MASK) >> LED_G_POS) << LED_GPS_SEGMENT_POS;
    aux |= ((cfg->ledRemoteOp & LED_G_MASK) >> LED_G_POS) << LED_REMOTE_OP_SEGMENT_POS;
    aux |= ((cfg->ledIsolated & LED_G_MASK) >> LED_G_POS) << LED_ISOLATED_SEGMENT_POS;
    as1116SetDigit(DIGIT_GREEN_POS, aux);

    aux = 0x00;
    aux |= ((cfg->ledOn & LED_B_MASK) >> LED_B_POS) << LED_ON_SEGMENT_POS;
    aux |= ((cfg->ledCt & LED_B_MASK) >> LED_B_POS) << LED_CT_SEGMENT_POS;
    aux |= ((cfg->ledFe & LED_B_MASK) >> LED_B_POS) << LED_FE_SEGMENT_POS;
    aux |= ((cfg->ledGps & LED_B_MASK) >> LED_B_POS) << LED_GPS_SEGMENT_POS;
    aux |= ((cfg->ledRemoteOp & LED_B_MASK) >> LED_B_POS) << LED_REMOTE_OP_SEGMENT_POS;
    aux |= ((cfg->ledIsolated & LED_B_MASK) >> LED_B_POS) << LED_ISOLATED_SEGMENT_POS;
    as1116SetDigit(DIGIT_BLUE_POS, aux);
}

/* ---------------------------- Global functions --------------------------- */

void ledPanelInit(void){

    as1116Init(GPIO6);

}


void ledPanelSetCfg(LedPanelCfg* cfg){

    rui8_t aux;

    if(cfg->digit0 < NUM_COUNT){
        aux = numeralCodes[cfg->digit0];
        if(cfg->pointPosition == 0){
            aux |= numeralCodes[PERIOD_ALPHA_POS];
        }
        cfg->digit0 = aux;
    } else {
        cfg->digit0 = 0x00;
    }

    if(cfg->digit1 < NUM_COUNT){
        aux = numeralCodes[cfg->digit1];
        if(cfg->pointPosition == 1){
            aux |= numeralCodes[PERIOD_ALPHA_POS];
        }
        cfg->digit1 = aux;
    } else {
        cfg->digit1 = 0x00;
    }

    if(cfg->digit2 < NUM_COUNT){
        aux = numeralCodes[cfg->digit2];
        if(cfg->pointPosition == 2){
            aux |= numeralCodes[PERIOD_ALPHA_POS];
        }
        cfg->digit2 = aux;
    } else {
        cfg->digit2 = 0x00;
    }

    if(cfg->digit3 < NUM_COUNT){
        aux = numeralCodes[cfg->digit3];
        if(cfg->pointPosition == 3){
            aux |= numeralCodes[PERIOD_ALPHA_POS];
        }
        cfg->digit3 = aux;
    } else {
        cfg->digit3 = 0x00;
    }

    ledPanelSetCfgInt(cfg);
}


void ledPanelTestSalt(void){

    LedPanelCfg testConfig;
    rui8_t aux;

    testConfig.pointPosition = 0;
    testConfig.digit0 = alphaCodes['t'-'a'] | digitCodeMap[PERIOD_ALPHA_POS];
    testConfig.digit1 = alphaCodes['l'-'a'];
    testConfig.digit2 = alphaCodes['a'-'a'];
    testConfig.digit3 = alphaCodes['s'-'a'];
    testConfig.ledOn = GREEN;
    testConfig.ledCt = YELLOW;
    testConfig.ledFe = RED;
    testConfig.ledGps = CYAN;
    testConfig.ledRemoteOp = BLUE;
    testConfig.ledIsolated = MAGENTA;

    while(1) {

        aux = testConfig.digit0;
        testConfig.digit0 = testConfig.digit1,
        testConfig.digit1 = testConfig.digit2;
        testConfig.digit2 = testConfig.digit3;
        testConfig.digit3 = aux;

        aux = testConfig.ledOn;
        testConfig.ledOn = testConfig.ledCt;
        testConfig.ledCt = testConfig.ledFe;
        testConfig.ledFe = testConfig.ledGps;
        testConfig.ledGps = testConfig.ledRemoteOp;
        testConfig.ledRemoteOp = testConfig.ledIsolated;
        testConfig.ledIsolated = aux;

        ledPanelSetCfgInt(&testConfig);

        gpioWrite( LED1, ON );

        delay(500);

        /* Apago el led azul */
        gpioWrite( LED1, OFF );

        delay(500);
    }

}

/* ------------------------------ End of file ------------------------------ */
