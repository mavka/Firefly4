/*
 * main.cpp
 *
 *  Created on: 26 дек. 2015 г.
 *      Author: Kreyl
 */

#include "main.h"
#include "SimpleSensors.h"

App_t App;

int main(void) {

    // ==== Setup clock frequency ====
//    uint8_t ClkResult = 1;
    Clk.EnablePrefetch();
    Clk.SetupBusDividers(ahbDiv2, apbDiv1);
    Clk.UpdateFreqValues();

    // Init OS
    halInit();
    chSysInit();

    // ==== Init hardware ====
    SYSCFG->CFGR1 |= SYSCFG_CFGR1_USART1TX_DMA_RMP | SYSCFG_CFGR1_USART1RX_DMA_RMP;
    Uart.Init(115200, UART_GPIO, UART_TX_PIN, UART_GPIO, UART_RX_PIN);
    Uart.Printf("\r%S %S\r", APP_NAME, APP_VERSION);
    Clk.PrintFreqs();
//    if(ClkResult != 0) Uart.Printf("XTAL failure\r");
    chThdSleepMilliseconds(450);

    App.InitThread();
    PinSensors.Init();

    // Main cycle
    App.ITask();
}

__attribute__ ((__noreturn__))
void App_t::ITask() {
    while(true) {
        uint32_t EvtMsk = chEvtWaitAny(ALL_EVENTS);
        if(EvtMsk & EVTMSK_USB_CONNECTED) {
            Uart.Printf("5v is here\r");
            chThdSleepMilliseconds(450);
            // Enable HSI48
            chSysLock();
            uint8_t r = Clk.SwitchTo(csHSI48);
            Clk.UpdateFreqValues();
            Uart.OnAHBFreqChange();
            chSysUnlock();
            if(r != OK) Uart.Printf("Hsi Fail\r");
            Clk.PrintFreqs();
        }
    } // while true
}

// 5v Sns
void Process5VSns(PinSnsState_t *PState, uint32_t Len) {
    if(PState[0] == pssRising) App.SignalEvt(EVTMSK_USB_CONNECTED);
    else if(PState[0] == pssFalling) App.SignalEvt(EVTMSK_USB_DISCONNECTED);
}
