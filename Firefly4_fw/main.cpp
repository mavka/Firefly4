/*
 * main.cpp
 *
 *  Created on: 26 дек. 2015 г.
 *      Author: Kreyl
 */

#include "main.h"
#include "SimpleSensors.h"
#include "color.h"
#include "usb_keybrd.h"
#include "kl_adc.h"

App_t App;

int main(void) {
    // ==== Setup clock frequency ====
    uint8_t rslt = 0;
    Clk.EnablePrefetch();
    Clk.SetupBusDividers(ahbDiv2, apbDiv1); // 48MHz/2 == 24MHz
    rslt = Clk.SwitchTo(csHSI48);
    Clk.UpdateFreqValues();

    // Init OS
    halInit();
    chSysInit();

    // ==== Init hardware ====
    SYSCFG->CFGR1 |= SYSCFG_CFGR1_USART1TX_DMA_RMP | SYSCFG_CFGR1_USART1RX_DMA_RMP; // Remap UART DMA
    Uart.Init(115200, UART_GPIO, UART_TX_PIN, UART_GPIO, UART_RX_PIN);
    Uart.Printf("\r%S %S\r", APP_NAME, APP_VERSION);
    Clk.PrintFreqs();
    App.InitThread();

    Adc.Init();
    PinSetupAnalog(SNS_GPIO, SNS_PIN0);
    PinSetupAnalog(SNS_GPIO, SNS_PIN1);

    UsbKBrd.Init();

    if(rslt == OK) {
        Clk.SelectUSBClock_HSI48();
        Clk.EnableCRS();
        UsbKBrd.Connect();
    }
    else Uart.Printf("Hsi Fail: %u\r", rslt);

    // Main cycle
    App.ITask();
}

__attribute__ ((__noreturn__))
void App_t::ITask() {
    TmrSampling.InitAndStart(PThread, MS2ST(SAMPLING_INTERVAL_MS), EVTMSK_SAMPLING, tvtPeriodic);
    TmrReset.Init(PThread, MS2ST(RESET_INTERVAL), EVTMSK_RESET, tvtOneShot);

    while(true) {
        uint32_t EvtMsk = chEvtWaitAny(ALL_EVENTS);

        if(EvtMsk & EVTMSK_USB_READY) {
            Uart.Printf("UsbReady\r");
        }

        if(EvtMsk & EVTMSK_UART_NEW_CMD) {
            OnCmd((Shell_t*)&Uart);
            Uart.SignalCmdProcessed();
        }

        // ==== ADC ====
        if(EvtMsk & EVTMSK_SAMPLING) {
            Adc.StartMeasurement();
        }
        if(EvtMsk & EVTMSK_ADC_DONE) {
            uint32_t Sns0 = Adc.GetResult(SNS_CHNL0);
            uint32_t Sns1 = Adc.GetResult(SNS_CHNL1);
            ProcessValues(Sns0, Sns1);
        }

        if(EvtMsk & EVTMSK_RESET) {
            ResetCounters();
        }
    } // while true
}

#if 1 // ======================= Signal processing =============================
void App_t::ProcessValues(uint32_t Sns0, uint32_t Sns1) {
//  Uart.Printf("%u; %u\r", Sns0, Sns1);
    // Normalize
    LowHigh_t Norm0, Norm1;
    Norm0 = Normalize(Sns0, Prev0);
    Norm1 = Normalize(Sns1, Prev1);
//    Uart.Printf("    %u; %u\r", Norm0, Norm1);
    // Detect edge
    RiseFall_t Edge0 = DetectEdge(Norm0, Prev0);
    RiseFall_t Edge1 = DetectEdge(Norm1, Prev1);
    // Detect gesture
    if((Edge0 == rfRising and Norm1 == Low) or (Edge0 == rfFalling and Norm1 == High)) CntD++;
    else if((Edge1== rfRising and Norm0 == Low) or (Edge1 == rfFalling and Norm0 == High)) CntU++;
    // Save current values as previous
    Prev0 = Norm0;
    Prev1 = Norm1;
    // Start timer
    if(CntD == 1 or CntU == 1) TmrReset.StartIfNotRunning();
    // Send event if gesture recognized
    if(CntD >= 2) {
        ResetCounters();
        Uart.Printf("Down\r");
        UsbKBrd.PressAndRelease(HID_KEYBOARD_SC_A);
    }
    else if(CntU >= 2) {
        ResetCounters();
        Uart.Printf("Up\r");
//        UsbKBrd.PressAndRelease(HID_KEYBOARD_SC_A);
    }
}

LowHigh_t App_t::Normalize(uint32_t X, LowHigh_t PrevX) {
    if(PrevX == Low) return (X > SNS_HIGH_THRESHOLD)? High : Low;
    else return (X < SNS_LOW_THRESHOLD)? Low : High;
}

RiseFall_t App_t::DetectEdge(LowHigh_t X, LowHigh_t PrevX) {
    if(X > PrevX) return rfRising;
    else if(X < PrevX) return rfFalling;
    else return rfNone;
}
#endif

#if 1 // ======================= Command processing ============================
void App_t::OnCmd(Shell_t *PShell) {
    Cmd_t *PCmd = &PShell->Cmd;
    __attribute__((unused)) int32_t dw32 = 0;  // May be unused in some configurations
//    PShell->Printf(">%S\r", PCmd->Name);
//    Uart.Printf("%S\r", PCmd->Name);
    // Handle command
    if(PCmd->NameIs("Ping")) {
        PShell->Ack(OK);
        UsbKBrd.PressAndRelease(HID_KEYBOARD_SC_A);
    }

    else PShell->Ack(CMD_UNKNOWN);
}
#endif
