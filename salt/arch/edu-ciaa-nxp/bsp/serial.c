/**
 *  \file       serial.c
 *  \brief      serial interface driver.
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
#include "serial.h"
#include "sapi.h"
#include "bsp.h"

/* ----------------------------- Local macros ------------------------------ */
/* ------------------------------- Constants ------------------------------- */

#define UART_TELOC_1500_LPC         LPC_USART0  /* UART0 (RS485/Profibus) */
#define UART_TELOC_1500_BAUDRATE    115200
#define UART_TELOC_1500_IRQ         USART0_IRQn
#define UART_TELOC_1500_IRQ_HANDLER UART0_IRQHandler

#define UART_SIM_808_B_LPC          LPC_USART2  /* UART2 (RS232) */
#define UART_SIM_808_B_BAUDRATE     19200
#define UART_SIM_808_B_IRQ          USART2_IRQn

#define UART_SIM_808_A_LPC          LPC_USART3  /* UART3 (RS232) */
#define UART_SIM_808_A_BAUDRATE     19200
#define UART_SIM_808_A_IRQ          USART3_IRQn
#define UART_SIM_808_A_IRQ_HANDLER  UART3_IRQHandler

#define UART_DEBUG_BAUDRATE     19200
#ifdef DEBUG_SERIAL_ARDUINO
#define UART_DEBUG_LPC          LPC_USART3  /* UART3 (RS232) */
#define UART_DEBUG_IRQ          USART3_IRQn
#else
#define UART_DEBUG_LPC          LPC_USART2  /* UART2 (RS232) */
#define UART_DEBUG_IRQ          USART2_IRQn
#endif

#define UART_SIM_808_B_OR_DEBUG_IRQ_HANDLER  UART2_IRQHandler
/* ---------------------------- Local data types --------------------------- */
/* ---------------------------- Global variables --------------------------- */
/* ---------------------------- Local variables ---------------------------- */
static serialIsrCb_t uartTelocCb = NULL;
static serialIsrCb_t uartSimACb = NULL;
static serialIsrCb_t uartSimBCb = NULL;
static serialIsrCb_t uartDebugCb = NULL;

static bool_t uartDebugInit = false;
/* ----------------------- Local function prototypes ----------------------- */
/* ---------------------------- Local functions ---------------------------- */
/* ---------------------------- Global functions --------------------------- */

void serialInit(serialMap_t serialMap){


    switch(serialMap){
        case UART_SIM_808_A:
            Chip_UART_Init(UART_SIM_808_A_LPC);
            Chip_UART_SetBaud(UART_SIM_808_A_LPC, UART_SIM_808_A_BAUDRATE);  /* Set Baud rate */
            Chip_UART_SetupFIFOS(UART_SIM_808_A_LPC, UART_FCR_FIFO_EN | UART_FCR_TRG_LEV0); /* Modify FCR (FIFO Control Register)*/
            Chip_UART_TXEnable(UART_SIM_808_A_LPC); /* Enable UART Transmission */
            Chip_SCU_PinMux(2, 3, MD_PDN, FUNC2);              /* P2_3,FUNC2: UART3_TXD */
            Chip_SCU_PinMux(2, 4, MD_PLN|MD_EZI|MD_ZI, FUNC2); /* P2_4,FUNC2: UART3_RXD */

            Chip_UART_IntEnable(UART_SIM_808_A_LPC, UART_IER_RBRINT ); //Enable UART Rx Interrupt
            Chip_UART_IntEnable(UART_SIM_808_A_LPC, UART_IER_RLSINT ); // Enable UART line status interrupt
            NVIC_SetPriority(UART_SIM_808_A_IRQ, 6);
            NVIC_EnableIRQ(UART_SIM_808_A_IRQ); // Enable Interrupt for UART channel
            break;

        case UART_SIM_808_B:
            Chip_UART_Init(UART_SIM_808_B_LPC);
            Chip_UART_SetBaud(UART_SIM_808_B_LPC, UART_SIM_808_B_BAUDRATE);  /* Set Baud rate */
            Chip_UART_SetupFIFOS(UART_SIM_808_B_LPC, UART_FCR_FIFO_EN | UART_FCR_TRG_LEV0); /* Modify FCR (FIFO Control Register)*/
            Chip_UART_TXEnable(UART_SIM_808_B_LPC); /* Enable UART Transmission */
            Chip_SCU_PinMux(1, 15, MD_PDN, FUNC1);              /* P1_15,FUNC1: UART2_TXD */
            Chip_SCU_PinMux(1, 16, MD_PLN|MD_EZI|MD_ZI, FUNC1); /* P1_16,FUNC1: UART2_RXD */

            Chip_UART_IntEnable(UART_SIM_808_B_LPC, UART_IER_RBRINT ); //Enable UART Rx Interrupt
            Chip_UART_IntEnable(UART_SIM_808_B_LPC, UART_IER_RLSINT ); // Enable UART line status interrupt
            NVIC_SetPriority(UART_SIM_808_B_IRQ, 6);
            NVIC_EnableIRQ(UART_SIM_808_B_IRQ); // Enable Interrupt for UART channel
            break;

        case UART_TELOC_1500:
            Chip_UART_Init(UART_TELOC_1500_LPC);
            Chip_UART_SetBaud(UART_TELOC_1500_LPC, UART_TELOC_1500_BAUDRATE);  /* Set Baud rate */
            Chip_UART_SetupFIFOS(UART_TELOC_1500_LPC, UART_FCR_FIFO_EN | UART_FCR_TRG_LEV0); /* Modify FCR (FIFO Control Register)*/
            Chip_UART_TXEnable(UART_TELOC_1500_LPC); /* Enable UART Transmission */
            //Chip_UART_SetRS485Flags()
            Chip_SCU_PinMux(6, 2, MD_PDN, FUNC1);              /* P6_2,FUNC1: UART0_DIR */
            Chip_SCU_PinMux(9, 5, MD_PDN, FUNC7);              /* P9_5,FUNC6: UART0_TXD */
            Chip_SCU_PinMux(9, 6, MD_PLN|MD_EZI|MD_ZI, FUNC7); /* P9_6,FUNC6: UART0_RXD */

            Chip_UART_IntEnable(UART_TELOC_1500_LPC, UART_IER_RBRINT ); //Receiver Buffer Register Interrupt
            Chip_UART_IntEnable(UART_TELOC_1500_LPC, UART_IER_RLSINT ); // Enable UART line status interrupt
            NVIC_SetPriority(UART_TELOC_1500_IRQ, 6);
            NVIC_EnableIRQ(UART_TELOC_1500_IRQ); // Enable Interrupt for UART channel
            break;
            
        case UART_DEBUG:
            uartDebugInit = true;
            Chip_UART_Init(UART_DEBUG_LPC);
            Chip_UART_SetBaud(UART_DEBUG_LPC, UART_DEBUG_BAUDRATE);  /* Set Baud rate */
            Chip_UART_SetupFIFOS(UART_DEBUG_LPC, UART_FCR_FIFO_EN | UART_FCR_TRG_LEV0); /* Modify FCR (FIFO Control Register)*/
            Chip_UART_TXEnable(UART_DEBUG_LPC); /* Enable UART Transmission */
#ifdef DEBUG_SERIAL_ARDUINO
            Chip_SCU_PinMux(2, 3, MD_PDN, FUNC2);              /* P2_3,FUNC2: UART3_TXD */
            Chip_SCU_PinMux(2, 4, MD_PLN|MD_EZI|MD_ZI, FUNC2); /* P2_4,FUNC2: UART3_RXD */
#else
            Chip_SCU_PinMux(7, 1, MD_PDN, FUNC6);              /* P7_1,FUNC6: UART2_TXD */
            Chip_SCU_PinMux(7, 2, MD_PLN|MD_EZI|MD_ZI, FUNC6); /* P7_2,FUNC6: UART2_RXD */
#endif

            Chip_UART_IntEnable(UART_DEBUG_LPC, UART_IER_RBRINT ); //Enable UART Rx Interrupt
            Chip_UART_IntEnable(UART_DEBUG_LPC, UART_IER_RLSINT ); // Enable UART line status interrupt
            NVIC_SetPriority(UART_DEBUG_IRQ, 6);
            NVIC_EnableIRQ(UART_DEBUG_IRQ); // Enable Interrupt for UART channel




            break;


        default:
            break;
    }
}

void serialSetIntCb(serialMap_t serialMap, serialIsrCb_t cb ){

    switch(serialMap){
        case UART_SIM_808_A:
            uartSimACb = cb;
            break;
        case UART_SIM_808_B:
            uartSimBCb = cb;
            break;
        case UART_TELOC_1500:
            uartTelocCb = cb;
            break;
        case UART_DEBUG:
            uartDebugCb = cb;
            break;

        default:
            break;
    }
}

void serialPutByte( serialMap_t uart, uint8_t byte ){

    switch(uart){
        case UART_SIM_808_A:
            while ((Chip_UART_ReadLineStatus(UART_SIM_808_A_LPC) & UART_LSR_THRE) == 0) {}   // Wait for space in FIFO
            Chip_UART_SendByte(UART_SIM_808_A_LPC, byte);
            break;
        case UART_SIM_808_B:
            while ((Chip_UART_ReadLineStatus(UART_SIM_808_B_LPC) & UART_LSR_THRE) == 0) {}   // Wait for space in FIFO
            Chip_UART_SendByte(UART_SIM_808_B_LPC, byte);
            break;
        case UART_TELOC_1500:
            while ((Chip_UART_ReadLineStatus(UART_TELOC_1500_LPC) & UART_LSR_THRE) == 0) {}   // Wait for space in FIFO
            Chip_UART_SendByte(UART_TELOC_1500_LPC, byte);
            break;
        case UART_DEBUG:
            if(!uartDebugInit){
                break;
            }
            while ((Chip_UART_ReadLineStatus(UART_DEBUG_LPC) & UART_LSR_THRE) == 0) {}   // Wait for space in FIFO
            Chip_UART_SendByte(UART_DEBUG_LPC, byte);
            break;

        default:
            break;
    }

}

void serialPutString( serialMap_t uart,  char *p ){
    while(*p!='\0')
    {
        serialPutByte(uart, *p);
        ++p;
    }
}

void serialPutChars( serialMap_t uart,  unsigned char *p, ruint ndata ){
    while(ndata && (ndata-- != 0))
    {
        serialPutByte(uart, *p);
        ++p;
    }
}

void UART_TELOC_1500_IRQ_HANDLER(void){
    char receivedByte;

    if (Chip_UART_ReadLineStatus(UART_TELOC_1500_LPC) & UART_LSR_RDR)
    {
        receivedByte = Chip_UART_ReadByte(UART_TELOC_1500_LPC);
        if(uartTelocCb != NULL)
            (uartTelocCb)(receivedByte);
    }
}

void UART_SIM_808_B_OR_DEBUG_IRQ_HANDLER(void){
    char receivedByte;

    if (Chip_UART_ReadLineStatus(UART_SIM_808_B_LPC) & UART_LSR_RDR)
    {
        receivedByte = Chip_UART_ReadByte(UART_SIM_808_B_LPC);
        if(uartSimBCb != NULL){
            (uartSimBCb)(receivedByte);
        }
        if(uartDebugCb != NULL){
            (uartDebugCb)(receivedByte);
        }
    }
}

void UART_SIM_808_A_IRQ_HANDLER(void){
    char receivedByte;

    if (Chip_UART_ReadLineStatus(UART_SIM_808_A_LPC) & UART_LSR_RDR)
    {
        receivedByte = Chip_UART_ReadByte(UART_SIM_808_A_LPC);
        if(uartSimACb != NULL)
            (uartSimACb)(receivedByte);
    }
}

/* ------------------------------ End of file ------------------------------ */
