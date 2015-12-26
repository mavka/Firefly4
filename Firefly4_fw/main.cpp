/*
 * main.cpp
 *
 *  Created on: 26 дек. 2015 г.
 *      Author: Kreyl
 */

#include "main.h"

App_t App;

int main(void) {

    // ==== Setup clock frequency ====
//    uint8_t ClkResult = 1;
//    SetupVCore(vcore1V8);
//    Clk.SetupFlashLatency(32);  // Setup Flash Latency for clock in MHz
//    Clk.SetupPLLMulDiv(pllMul8, pllDiv3);
//    Clk.SetupBusDividers(ahbDiv1, apbDiv1, apbDiv1);
//    ClkResult = Clk.SwitchToPLL();
    Clk.UpdateFreqValues();

    // Init OS
    halInit();
    chSysInit();

    // ==== Init hardware ====
    Uart.Init(256000, UART_GPIO, UART_TX_PIN, UART_GPIO, UART_RX_PIN);
    Uart.Printf("\r%S %S\r", APP_NAME, APP_VERSION);

//    Clk.PrintFreqs();
//    if(ClkResult != 0) Uart.Printf("XTAL failure\r");

    App.InitThread();

    // Main cycle
    App.ITask();
}

__attribute__ ((__noreturn__))
void App_t::ITask() {
    while(true) {
//        uint32_t EvtMsk = chEvtWaitAny(ALL_EVENTS);
        chThdSleepMilliseconds(999);
    } // while true
}
