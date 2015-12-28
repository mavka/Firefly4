/*
 * ws2812b.h
 *
 *  Created on: 05 апр. 2014 г.
 *      Author: Kreyl
 */

#ifndef WS2812B_H_
#define WS2812B_H_

#include "ch.h"
#include "hal.h"
#include "kl_lib.h"
#include "color.h"

#define LED_DMA_MODE    DMA_PRIORITY_VERYHIGH \
                        | STM32_DMA_CR_MSIZE_BYTE \
                        | STM32_DMA_CR_PSIZE_HWORD \
                        | STM32_DMA_CR_MINC     /* Memory pointer increase */ \
                        | STM32_DMA_CR_DIR_M2P  /* Direction is memory to peripheral */ \
                        | STM32_DMA_CR_TCIE     /* Enable Transmission Complete IRQ */

// Bit Buffer (no tuning required)
#define RST_BIT_CNT     45 // 45 zero bits to produce reset
#define DATA_BIT_CNT    (LED_CNT * 3 * 8)   // Each led has 3 channels 8 bit each
#define TOTAL_BIT_CNT   (DATA_BIT_CNT + RST_BIT_CNT)

// Tx timings
#define T0H_N       1
#define T1H_N       3
#define T_TOTAL_N   18

enum ClrSetupMode_t {csmOneByOne, csmSimultaneously};

class LedWs_t {
private:
    PinOutputPWM_t<T_TOTAL_N, invNotInverted, omPushPull> TxTmr{LEDWS_GPIO, LEDWS_PIN, LEDWS_TMR, LEDWS_TMR_CH};
    virtual_timer_t ITmr;
    uint8_t BitBuf[TOTAL_BIT_CNT], *PBit, Indx;
    Color_t IClr[LED_CNT];
    ClrSetupMode_t IMode;
    void AppendBitsMadeOfByte(uint8_t Byte);
    void ISetCurrentColors();
    uint32_t ICalcDelay(uint16_t AValue, uint32_t Smooth) { return (uint32_t)((Smooth / (AValue+4)) + 1); }
    uint32_t ICalcDelayForAll();
public:
    Color_t DesiredClr[LED_CNT];
    ftVoidVoid OnSmoothStart, OnSmoothEnd;
    void Init();
    void SetCommonColor(Color_t Clr);
    void SetCommonColorSmoothly(Color_t Clr, ClrSetupMode_t AMode = csmSimultaneously);
    // Inner use
    void IStopTx() { TxTmr.Set(0); TxTmr.Disable(); }
    void ITmrHandler();
};

extern LedWs_t LedWs;

#endif /* WS2812B_H_ */
