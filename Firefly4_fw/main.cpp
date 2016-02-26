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
    __unused uint8_t rslt = 0;
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

__noreturn
void App_t::ITask() {
    TmrSampling.InitAndStart(PThread, MS2ST(SAMPLING_INTERVAL_MS), EVTMSK_SAMPLING, tktPeriodic);

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
    } // while true
}

#if 1 // ======================= Signal processing =============================
enum RecState_t {rsIdle, rsEnter};
static RecState_t RecState = rsIdle;

static virtual_timer_t TmrTimeout;
static void RecCallback(void *p) {
    RecState = rsIdle;
//    Uart.PrintfI("TO\r");
}

__unused static uint32_t cnt = 0;

void App_t::ProcessValues(uint32_t Sns0, uint32_t Sns1) {
//    Uart.Printf("%03u %03u\r\n", Sns0, Sns1);
    Sns0 = (Sns0 > SNS_THRESHOLD)? 1 : 0;
    Sns1 = (Sns1 > SNS_THRESHOLD)? 1 : 0;

    if(RecState == rsIdle) {
        if(Sns0 == 1 and Sns1 == 0) {
//            Uart.Printf("Enter\r");
            RecState = rsEnter;
            chVTSet(&TmrTimeout, MS2ST(999), RecCallback, nullptr);
        }
    }
    // rsEnter
    else {
        if(Sns1 == 1) {
            Uart.Printf("%u\r", cnt++);
            RecState = rsIdle;
            chVTReset(&TmrTimeout);
//            Uart.Printf("Idle\r");
        }
    }
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
