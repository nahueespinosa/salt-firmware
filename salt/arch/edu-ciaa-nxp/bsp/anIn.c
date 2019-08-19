/**
 *  \file       anin.c
 *  \brief      Implementation of Analog Inputs adquisition and filtering.
 */

/* -------------------------- Development history -------------------------- */
/*
 *  2018.05.17  DaBa  v1.0.00  Initial version
 *  2019.06.17  IMD  v1.0.01  SALT changes
 */

/* -------------------------------- Authors -------------------------------- */
/*
 *  DaBa  Dario Baliña       db@vortexmakes.com
 *  IMD  Ivan Mariano Di Vito divitoivan@gmail.com.com
 */

/* --------------------------------- Notes --------------------------------- */
/* ----------------------------- Include files ----------------------------- */
#include "rkh.h"
#include "sapi.h"
#include "anIn.h"
#include "emaFilter.h"

/* ----------------------------- Local macros ------------------------------ */
/* ------------------------------- Constants ------------------------------- */
#define ANINS_EMA_ALPHA     2
#define SAPI_ADC_CH_OFFSET  66

#define ADCONV_RESOLUTION       10  /* In bits */
#define ADCONV_FULL_SCALE       3.3  /* In volts */
#define ADCONV_DIG_FRACTION     100 /* Number of digits in fractional part */
/* ---------------------------- Local data types --------------------------- */
/* ---------------------------- Global variables --------------------------- */
/* ---------------------------- Local variables ---------------------------- */
static adc_t anIns[NUM_ANIN_SIGNALS];
static const uint8_t chMap[NUM_ANIN_SIGNALS] = {CH1,};
static int currChannel;
onAnInCb_t onAnInCb;

/* ----------------------- Local function prototypes ----------------------- */
/* ---------------------------- Local functions ---------------------------- */
static void
anInAdcStart(int channel)
{
    uint8_t lpcAdcChannel = SAPI_ADC_CH_OFFSET - chMap[channel];

    Chip_ADC_EnableChannel(LPC_ADC0, lpcAdcChannel, ENABLE);
    Chip_ADC_SetStartMode(LPC_ADC0, ADC_START_NOW, ADC_TRIGGERMODE_RISING);
}

static uint16_t
anInAdcRead(int channel)
{   
    uint8_t lpcAdcChannel = SAPI_ADC_CH_OFFSET - chMap[channel];
    uint16_t analogValue = 0;

    while(Chip_ADC_ReadStatus(LPC_ADC0, lpcAdcChannel, ADC_DR_DONE_STAT) != SET){

    }

    Chip_ADC_ReadValue( LPC_ADC0, lpcAdcChannel, &analogValue );

    Chip_ADC_EnableChannel( LPC_ADC0, lpcAdcChannel, DISABLE );

    return analogValue;
}

sample_t
convertToSampleValue(adc_t sample)
{
    double value;
    value = ((double)(sample * ADCONV_FULL_SCALE) / (1 << ADCONV_RESOLUTION));
    return (sample_t)value;
}


/* ---------------------------- Global functions --------------------------- */
void
anInInit(onAnInCb_t cb)
{
    onAnInCb = cb;

    adcConfig(ADC_ENABLE);

    currChannel = anIn0;
    anInAdcStart(currChannel);
}

void
anInCaptureAndFilter(void)
{
    uint16_t value;

    value = anInAdcRead(currChannel);
    anIns[currChannel] = emaFilter_LowPass(value, 
                                           anIns[currChannel],
                                           ANINS_EMA_ALPHA);

    if(++currChannel >= NUM_ANIN_SIGNALS){
        currChannel = anIn0;

        if(onAnInCb != NULL){
            onAnInCb();
        }
    }

    anInAdcStart(currChannel);
}

adc_t
anInGet(int channel)
{
    if(channel >= NUM_ANIN_SIGNALS)
        return 0;

    return anIns[channel];
}


sample_t anInGetSample(int channel)
{
    return convertToSampleValue(anInGet(channel));
}

/* ------------------------------ End of file ------------------------------ */
