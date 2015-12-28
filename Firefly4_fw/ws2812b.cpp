/*
 * ws2812b.cpp
 *
 *  Created on: 05 апр. 2014 г.
 *      Author: Kreyl
 */

#include "ws2812b.h"
#include "uart.h"

LedWs_t LedWs;

extern "C" {
// Wrapper for Tx Completed IRQ
void LedTxcIrq(void *p, uint32_t flags) {
    dmaStreamDisable(LEDWS_DMA);
    LedWs.IStopTx();
    //Uart.Printf("Irq\r");
}
static inline void LedTmrCallback(void *p) { LedWs.ITmrHandler(); }
} // "C"

void LedWs_t::Init() {
    // ==== Timer ====
#if defined LED_REMAP_TIM15 // Remap T15CH2 to PB15
    bool AfioWasEnabled = (RCC->APB2ENR & RCC_APB2ENR_AFIOEN);
    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;     // Enable AFIO
    AFIO->MAPR2 |= AFIO_MAPR2_TIM15_REMAP;
    if (!AfioWasEnabled) RCC->APB2ENR &= ~RCC_APB2ENR_AFIOEN;
#endif
    // Init tmr in PWM mode
    TxTmr.Init();
    TxTmr.Set(0);
    TxTmr.EnableDmaOnUpdate();
    TxTmr.Disable();

    // Zero buffer
    for(uint32_t i=0; i<RST_BIT_CNT; i++) BitBuf[i] = 0;

    // ==== DMA ====
    dmaStreamAllocate     (LEDWS_DMA, IRQ_PRIO_LOW, LedTxcIrq, NULL);
    dmaStreamSetPeripheral(LEDWS_DMA, TMR_PCCR(LEDWS_TMR, LEDWS_TMR_CH));
    dmaStreamSetMemory0   (LEDWS_DMA, BitBuf);
    dmaStreamSetMode      (LEDWS_DMA, LED_DMA_MODE);
}

void LedWs_t::SetCommonColor(Color_t Clr) {
    for(uint32_t i=0; i<LED_CNT; i++) IClr[i] = Clr;
    ISetCurrentColors();
}

void LedWs_t::SetCommonColorSmoothly(Color_t Clr, ClrSetupMode_t AMode) {
    if(OnSmoothStart != nullptr) OnSmoothStart();
    chVTReset(&ITmr);
    IMode = AMode;
    for(uint32_t i=0; i<LED_CNT; i++) DesiredClr[i] = Clr;
    Indx = 0;   // Start with first LED
    ITmrHandler();
}

void LedWs_t::ITmrHandler() {
//    Uart.Printf("I1=%u\r", Indx);
    while(IClr[Indx] == DesiredClr[Indx]) {
        Indx++;
        if(Indx >= LED_CNT) {
            if(OnSmoothEnd != nullptr) OnSmoothEnd();
            return; // Setup completed
        }
    }
    uint32_t Delay = ICalcDelayForAll();
    // Adjust color(s) depending on mode
    if(IMode == csmOneByOne) IClr[Indx].Adjust(&DesiredClr[Indx]);
    else {  // Simultaneously
        for(uint8_t i=0; i<LED_CNT; i++) IClr[i].Adjust(&DesiredClr[Indx]);
    }
    ISetCurrentColors();
//    Uart.Printf("I=%u; D=%u\r", Indx, Delay);
    chVTSet(&ITmr, MS2ST(Delay), LedTmrCallback, NULL);
}

void LedWs_t::AppendBitsMadeOfByte(uint8_t Byte) {
    for(uint8_t i=0; i<8; i++) {
        if(Byte & 0x80) *PBit = T1H_N;
        else *PBit = T0H_N;
        PBit++;
        Byte <<= 1;
    }
}

void LedWs_t::ISetCurrentColors() {
    // Fill bit buffer
    PBit = &BitBuf[RST_BIT_CNT];
    for(uint32_t i=0; i<LED_CNT; i++) {
        AppendBitsMadeOfByte(IClr[i].G);
        AppendBitsMadeOfByte(IClr[i].R);
        AppendBitsMadeOfByte(IClr[i].B);
    }
    // Start transmission
    dmaStreamSetTransactionSize(LEDWS_DMA, TOTAL_BIT_CNT);
    dmaStreamEnable(LEDWS_DMA);
    TxTmr.SetCounter(0);

//    chSysLockFromIsr();
    TxTmr.Enable();
//    dmaWaitCompletion(LED_DMA_STREAM);
//    IStopTx();
//    chSysUnlockFromIsr();
}

uint32_t LedWs_t::ICalcDelayForAll() {
    uint32_t DelayR = (IClr[Indx].R == DesiredClr[Indx].R)? 0 : ICalcDelay(IClr[Indx].R, 810);  // XXX
    uint32_t DelayG = (IClr[Indx].G == DesiredClr[Indx].G)? 0 : ICalcDelay(IClr[Indx].G, 810);
    uint32_t DelayB = (IClr[Indx].B == DesiredClr[Indx].B)? 0 : ICalcDelay(IClr[Indx].B, 810);
//    Uart.Printf("I=%u; R=%u/%u; G=%u/%u; B=%u/%u\r",
//            Indx,
//            IClr[Indx].Red,   DesiredClr[Indx].Red,
//            IClr[Indx].Green, DesiredClr[Indx].Green,
//            IClr[Indx].Blue,  DesiredClr[Indx].Blue);
//    Uart.Printf("DR=%u; DG=%u; DB=%u\r", DelayR, DelayG, DelayB);
    uint32_t Rslt = DelayR;
    if(DelayG > Rslt) Rslt = DelayG;
    if(DelayB > Rslt) Rslt = DelayB;
    return Rslt;
}
