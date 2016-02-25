/*
 * led_rgb.cpp
 *
 *  Created on: 31 рту. 2014 у.
 *      Author: Kreyl
 */

#include <led_rgb_f100.h>
#include "uart.h"

LedRGB_t Led;

// Timer callback
static void LedTmrCallback(void *p) {
    chSysLockFromIsr();
    Led.IStartSequenceI((const LedChunk_t*)p);
    chSysUnlockFromIsr();
}

void LedRGB_t::Init() {
    R.Init();
    G.Init();
    B.Init();
    SetColor(clBlack);
}

void LedRGB_t::IStartSequenceI(const LedChunk_t *PLedChunk) {
    // Reset timer
    if(chVTIsArmedI(&ITmr)) chVTResetI(&ITmr);
    // Process the sequence
    while(PLedChunk != nullptr) {
//        Uart.Printf("\rCh %u", PLedChunk->ChunkSort);
        switch(PLedChunk->ChunkSort) {
            case csSetColor:
                if(ICurrColor != PLedChunk->Color) {
                    if(PLedChunk->MorphMS == 0) {   // If smooth time is zero,
                        SetColor(PLedChunk->Color); // set color now,
                        PLedChunk++;                // and goto next chunk
                    }
                    else {
                        // Adjust color
                        ICurrColor.Adjust(&PLedChunk->Color);
                        ISetCurrent();
                        // Check if completed now
                        if(ICurrColor == PLedChunk->Color) PLedChunk++;
                        else { // Not completed
                            // Calculate time to next adjustment
                            uint32_t DelayR = (ICurrColor.R == PLedChunk->Color.R)? 0 : ICalcDelay(ICurrColor.R, PLedChunk->MorphMS);
                            uint32_t DelayG = (ICurrColor.G == PLedChunk->Color.G)? 0 : ICalcDelay(ICurrColor.G, PLedChunk->MorphMS);
                            uint32_t DelayB = (ICurrColor.B == PLedChunk->Color.B)? 0 : ICalcDelay(ICurrColor.B, PLedChunk->MorphMS);
                            uint32_t Delay = DelayR;
                            if(DelayG > Delay) Delay = DelayG;
                            if(DelayB > Delay) Delay = DelayB;
                            chVTSetI(&ITmr, MS2ST(Delay), LedTmrCallback, (void*)PLedChunk);
                            return;
                        } // Not completed
                    } // if time > 256
                } // if color is different
                else PLedChunk++; // Color is the same, goto next chunk
                break;

            case csWait: // Start timer, pointing to next chunk
                if(PLedChunk->Time_ms == 0) PLedChunk++;
                else {
                    chVTSetI(&ITmr, MS2ST(PLedChunk->Time_ms), LedTmrCallback, (void*)(PLedChunk+1));
                    return;
                }
                break;

            case csGoto:
                PLedChunk = IPStartChunk + PLedChunk->ChunkToJumpTo;
                chVTSetI(&ITmr, MS2ST(1), LedTmrCallback, (void*)PLedChunk);    // To not allow infinite cycle
                return;
                break;

            case csEnd:
                IPStartChunk = nullptr;
                return;
                break;
        } // switch
    } // while
}

#if 1 // ============================= LED Channel =============================
void LedChnl_t::Init() const {
    // ==== GPIO setup ====
    PinSetupAlterFunc(PGpio, Pin, omPushPull);
    // ==== Timer setup ====
    if     (PTimer == TIM2)  { rccEnableTIM2(FALSE); }
    else if(PTimer == TIM3)  { rccEnableTIM3(FALSE); }
    PTimer->CR1 = TIM_CR1_CEN; // Enable timer, set clk division to 0, AutoReload not buffered
    PTimer->CR2 = 0;
    PTimer->ARR = LED_TOP_VALUE;
    // ==== Timer's channel ====
#if LED_INVERTED_PWM
#define PwmMode 0b111
#else
#define PwmMode 0b110
#endif
    switch(TmrChnl) {
        case 1:
            PTimer->CCMR1 |= (PwmMode << 4);
            PTimer->CCER  |= TIM_CCER_CC1E;
            break;
        case 2:
            PTimer->CCMR1 |= (PwmMode << 12);
            PTimer->CCER  |= TIM_CCER_CC2E;
            break;
        case 3:
            PTimer->CCMR2 |= (PwmMode << 4);
            PTimer->CCER  |= TIM_CCER_CC3E;
            break;
        case 4:
            PTimer->CCMR2 |= (PwmMode << 12);
            PTimer->CCER  |= TIM_CCER_CC4E;
            break;
        default: break;
    }
}
#endif
