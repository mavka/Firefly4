/*
 * cmd_uart.cpp
 *
 *  Created on: 15.04.2013
 *      Author: kreyl
 */

#include <string.h>
#include "uart.h"
#include "main.h" // App_t

Uart_t Uart;

#if UART_USE_DMA
// Wrapper for TX IRQ
extern "C" {
void CmdUartTxIrq(void *p, uint32_t flags) { Uart.IRQDmaTxHandler(); }
}

// ==== TX DMA IRQ ====
void Uart_t::IRQDmaTxHandler() {
    dmaStreamDisable(UART_DMA_TX);    // Registers may be changed only when stream is disabled
    IFullSlotsCount -= ITransSize;
    PRead += ITransSize;
    if(PRead >= (TXBuf + UART_TXBUF_SZ)) PRead = TXBuf; // Circulate pointer

    if(IFullSlotsCount == 0) IDmaIsIdle = true; // Nothing left to send
    else ISendViaDMA();
}

extern "C" {
void PrintfC(const char *format, ...) {
//    chSysLock();
    va_list args;
    va_start(args, format);
    Uart.IPrintf(format, args);
    va_end(args);
//    chSysUnlock();
}
}

static inline void FPutChar(char c) { Uart.IPutChar(c); }

void Uart_t::Printf(const char *format, ...) {
    chSysLock();
    va_list args;
    va_start(args, format);
    IPrintf(format, args);
    va_end(args);
    chSysUnlock();
}

void Uart_t::PrintfI(const char *format, ...) {
    va_list args;
    va_start(args, format);
    IPrintf(format, args);
    va_end(args);
}

void Uart_t::IPutChar(char c) {
    *PWrite++ = c;
    if(PWrite >= &TXBuf[UART_TXBUF_SZ]) PWrite = TXBuf;   // Circulate buffer
}

void Uart_t::IPrintf(const char *format, va_list args) {
    int32_t MaxLength = UART_TXBUF_SZ - IFullSlotsCount;
    IFullSlotsCount += kl_vsprintf(FPutChar, MaxLength, format, args);
    // Start transmission if Idle
    if(IDmaIsIdle) ISendViaDMA();
}

void Uart_t::ISendViaDMA() {
    uint32_t PartSz = (TXBuf + UART_TXBUF_SZ) - PRead; // Cnt from PRead to end of buf
    ITransSize = MIN(IFullSlotsCount, PartSz);
    if(ITransSize != 0) {
        IDmaIsIdle = false;
        dmaStreamSetMemory0(UART_DMA_TX, PRead);
        dmaStreamSetTransactionSize(UART_DMA_TX, ITransSize);
        dmaStreamSetMode(UART_DMA_TX, UART_DMA_TX_MODE);
        dmaStreamEnable(UART_DMA_TX);
    }
}
#endif

#if 1 // ==== Print Now ====
static inline void FPutCharNow(char c) {
#if defined STM32L1XX || defined STM32F2XX || defined STM32F4XX || defined STM32F10X_LD_VL
    while(!(UART->SR & USART_SR_TXE));
    UART_TX_REG = c;
    while(!(UART->SR & USART_SR_TXE));
#elif defined STM32F030
    while(!(UART->ISR & USART_ISR_TXE));
    UART_TX_REG = c;
    while(!(UART->ISR & USART_ISR_TXE));
#endif
}

void Uart_t::PrintfNow(const char *S, ...) {
    va_list args;
    va_start(args, S);
    kl_vsprintf(FPutCharNow, 99999, S, args);
    va_end(args);
}

extern "C" {
void PrintfCNow(const char *format, ...) {
    va_list args;
    va_start(args, format);
    kl_vsprintf(FPutCharNow, 99999, format, args);
    va_end(args);
}
}

#endif

#if UART_RX_ENABLED
__attribute__((__noreturn__))
void Uart_t::IRxTask() {
    while(true) {
        chThdSleepMilliseconds(UART_RX_POLLING_MS);
        // Get number of bytes to process
#if defined STM32F2XX || defined STM32F4XX
        int32_t Sz = UART_RXBUF_SZ - UART_DMA_RX->stream->NDTR;   // Number of bytes copied to buffer since restart
#else
        int32_t Sz = UART_RXBUF_SZ - UART_DMA_RX->channel->CNDTR;   // Number of bytes copied to buffer since restart
#endif
        if(Sz != SzOld) {
            int32_t ByteCnt = Sz - SzOld;
            if(ByteCnt < 0) ByteCnt += UART_RXBUF_SZ;   // Handle buffer circulation
            SzOld = Sz;
            // Iterate received bytes
            for(int32_t i=0; i<ByteCnt; i++) {
                char c = IRxBuf[RIndx++];
                if(RIndx >= UART_RXBUF_SZ) RIndx = 0;
                if(Cmd.PutChar(c) == pdrNewCmd) {
                    chSysLock();
                    App.SignalEvtI(EVTMSK_UART_NEW_CMD);
                    chSchGoSleepS(THD_STATE_SUSPENDED); // Wait until cmd processed
                    chSysUnlock();  // Will be here when application signals that cmd processed
                }
            } // for
        } // if sz
    } // while true
}

static WORKING_AREA(waUartRxThread, 128);
__attribute__((__noreturn__))
static void UartRxThread(void *arg) {
    chRegSetThreadName("UartRx");
    Uart.IRxTask();
}
#endif

// ==== Init & DMA ====
#if UART_RX_ENABLED
void Uart_t::Init(uint32_t ABaudrate, GPIO_TypeDef *PGpioTx, const uint16_t APinTx, GPIO_TypeDef *PGpioRx, const uint16_t APinRx) {
#else
void Uart_t::Init(uint32_t ABaudrate, GPIO_TypeDef *PGpioTx, const uint16_t APinTx) {
#endif
    PinSetupAlterFunc(PGpioTx, APinTx, omPushPull);
    IBaudrate = ABaudrate;
    // ==== USART configuration ====
    if(UART == USART1) {rccEnableUSART1(FALSE); }
    else if(UART == USART2) {rccEnableUSART2(FALSE); }

    OnAHBFreqChange();  // Setup baudrate

    UART->CR2 = 0;
#if UART_USE_DMA    // ==== DMA ====
    dmaStreamAllocate     (UART_DMA_TX, IRQ_PRIO_MEDIUM, CmdUartTxIrq, NULL);
    dmaStreamSetPeripheral(UART_DMA_TX, &UART_TX_REG);
    dmaStreamSetMode      (UART_DMA_TX, UART_DMA_TX_MODE);
    IDmaIsIdle = true;
#endif

#if UART_RX_ENABLED
    UART->CR1 = USART_CR1_TE | USART_CR1_RE;        // TX & RX enable
    UART->CR3 = USART_CR3_DMAT | USART_CR3_DMAR;    // Enable DMA at TX & RX

    PinSetupIn(PGpioRx, APinRx, pudPullUp);

    dmaStreamAllocate     (UART_DMA_RX, IRQ_PRIO_LOW, nullptr, NULL);
    dmaStreamSetPeripheral(UART_DMA_RX, &UART_RX_REG);
    dmaStreamSetMemory0   (UART_DMA_RX, IRxBuf);
    dmaStreamSetTransactionSize(UART_DMA_RX, UART_RXBUF_SZ);
    dmaStreamSetMode      (UART_DMA_RX, UART_DMA_RX_MODE);
    dmaStreamEnable       (UART_DMA_RX);
    // Thread
    IPThd = chThdCreateStatic(waUartRxThread, sizeof(waUartRxThread), LOWPRIO, (tfunc_t)UartRxThread, NULL);
#else
    UART->CR1 = USART_CR1_TE;     // Transmitter enabled
#if UART_USE_DMA
    UART->CR3 = USART_CR3_DMAT;   // Enable DMA at transmitter
#endif
#endif
    UART->CR1 |= USART_CR1_UE;    // Enable USART
}

void Uart_t::OnAHBFreqChange() {
#if defined STM32L1XX || defined STM32F100_MCUCONF
    if(UART == USART1) UART->BRR = Clk.APB2FreqHz / IBaudrate;
    else               UART->BRR = Clk.APB1FreqHz / IBaudrate;
#elif defined STM32F030
    UART->BRR = Clk.APBFreqHz / IBaudrate;
#elif defined STM32F2XX || defined STM32F4XX
    if(UART == USART1 or UART == USART6)
        UART->BRR = Clk.APB2FreqHz / IBaudrate;
    else
        UART->BRR = Clk.APB1FreqHz / IBaudrate;
#endif
}
