/*
 * led_rgb.h
 *
 *  Created on: 31 рту. 2014 у.
 *      Author: Kreyl
 */

#ifndef LED_RGB_H_
#define LED_RGB_H_

#include "hal.h"
#include "kl_lib.h"
#include "color.h"
#include "LedSequence.h"

// ==== LED's ports, pins and Tmr ====
struct LedChnl_t {
    GPIO_TypeDef *PGpio;
    uint16_t Pin;
    TIM_TypeDef *PTimer;
    uint32_t TmrChnl;
    void Set(const uint8_t AValue) const { *(uint16_t*)(&PTimer->CCR1 + (TmrChnl-1)*2) = AValue; }    // CCR[N] = AValue
    void Init() const;
};

// Port, pin and timer settings. Edit this properly.
#define LED_TOP_VALUE       255
#define LED_INVERTED_PWM    FALSE
const LedChnl_t
    R = {GPIOA, 2, TIM2, 3},
    G = {GPIOA, 3, TIM2, 4},
    B = {GPIOA, 1, TIM2, 2};

// TimeToWaitBeforeNextAdjustment = SmoothVar / (N+4) + 1, where N - current LED brightness.
// ==== LedRGB itself ====
class LedRGB_t {
private:
    const LedChunk_t *IPStartChunk;
    uint32_t ICalcDelay(uint32_t CurrentBrightness, uint32_t SmoothVar) { return (uint32_t)((SmoothVar / (CurrentBrightness+4)) + 1); }
    VirtualTimer ITmr;
    Color_t ICurrColor;
    void ISetCurrent() {
        R.Set(ICurrColor.R);
        G.Set(ICurrColor.G);
        B.Set(ICurrColor.B);
    }
public:
    void Init();
    void SetColor(Color_t AColor) {
        R.Set(AColor.R);
        G.Set(AColor.G);
        B.Set(AColor.B);
        ICurrColor = AColor;
    }
    void StartSequence(const LedChunk_t *PLedChunk) {
        chSysLock();
        IPStartChunk = PLedChunk;   // Save first chunk
        IStartSequenceI(PLedChunk);
        chSysUnlock();
    }
    void Stop() {
        chSysLock();
        if(chVTIsArmedI(&ITmr)) chVTResetI(&ITmr);
        SetColor(clBlack);
        chSysUnlock();
    }
    const LedChunk_t* GetCurrentSequence() { return IPStartChunk; }
    // Inner use
    void IStartSequenceI(const LedChunk_t *PLedChunk);
};

extern LedRGB_t Led;

#endif /* LED_RGB_H_ */
