/*
 * main.cpp
 *
 *  Created on: 26 дек. 2015 г.
 *      Author: Kreyl
 */

#include "main.h"
#include "SimpleSensors.h"
#include "usb_cdc.h"
#include "color.h"
#include "ws2812b.h"

App_t App;

class LedParams_t {
public:
    union {
        struct {
            int32_t Indx;
            int32_t R, G, B;
            int32_t Smooth;
        };
        int32_t Arr[5];
    };
    uint8_t Check(Shell_t *PShell) {
        uint8_t Rslt = OK;
        if(Indx < 1 or Indx > 3) {
            PShell->Printf("Bad indx: %d\r\n", Indx);
            Rslt = FAILURE;
        }
        if(R < 0 or R > 255) {
            PShell->Printf("Bad R: %d\r\n", R);
            Rslt = FAILURE;
        }
        if(G < 0 or G > 255) {
            PShell->Printf("Bad G: %d\r\n", G);
            Rslt = FAILURE;
        }
        if(B < 0 or B > 255) {
            PShell->Printf("Bad B: %d\r\n", B);
            Rslt = FAILURE;
        }
        if(Smooth < 0) {
            PShell->Printf("Bad Smooth: %d\r\n", Smooth);
            Rslt = FAILURE;
        }
        return Rslt;
    }
    void Print() {
        Uart.Printf("%u  %u %u %u   %u\r", Indx, R, G, B, Smooth);
    }
};

LedParams_t LedParams[LED_CNT];

int main(void) {
    // ==== Setup clock frequency ====
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
    chThdSleepMilliseconds(45);

    App.InitThread();
    PinSensors.Init();
    UsbCDC.Init();
    LedWs.Init();
    LedWs.SetCommonColor(clGreen);

    // Main cycle
    App.ITask();
}

__attribute__ ((__noreturn__))
void App_t::ITask() {
    while(true) {
        uint32_t EvtMsk = chEvtWaitAny(ALL_EVENTS);

//        if(EvtMsk & EVTMSK_USB_CONNECTED) {
//            Uart.Printf("5v is here\r");
//            chThdSleepMilliseconds(450);
//            // Enable HSI48
//            chSysLock();
//            uint8_t r = Clk.SwitchTo(csHSI48);
//            Clk.UpdateFreqValues();
//            Uart.OnAHBFreqChange();
//            chSysUnlock();
//            Clk.PrintFreqs();
//            if(r == OK) {
//                Clk.SelectUSBClock_HSI48();
//                Clk.EnableCRS();
//                UsbCDC.Connect();
//            }
//            else Uart.Printf("Hsi Fail\r");
//        }

        if(EvtMsk & EVTMSK_USB_READY) {
            Uart.Printf("UsbReady\r");
        }

        if(EvtMsk & EVTMSK_USB_NEW_CMD) {
            OnCmd((Shell_t*)&UsbCDC);
            UsbCDC.SignalCmdProcessed();
        }
        if(EvtMsk & EVTMSK_UART_NEW_CMD) {
            OnCmd((Shell_t*)&Uart);
            Uart.SignalCmdProcessed();
        }
    } // while true
}

#if 1 // ======================= Command processing ============================
void App_t::OnCmd(Shell_t *PShell) {
    Cmd_t *PCmd = &PShell->Cmd;
    __attribute__((unused)) int32_t dw32 = 0;  // May be unused in some configurations
    PShell->Printf(">%S\r", PCmd->Name);
//    Uart.Printf("%S\r", PCmd->Name);
    // Handle command
    if(PCmd->NameIs("Ping")) {
        PShell->Ack(OK);
    }

    else if(PCmd->NameIs("Set")) {
        uint8_t Rslt = CMD_ERROR;
        for(uint8_t i = 0; i < 3; i++) {
            // Get array of params
            if(PCmd->GetArray(LedParams[i].Arr, 5) == OK) {
                LedParams[i].Print();
                if(LedParams[i].Check(PShell) == OK) {
                    Rslt = OK;
                }
                else break;
            }
            else break;
        } // for
        PShell->Ack(Rslt);
    }

    else PShell->Ack(CMD_UNKNOWN);
}
#endif

// 5v Sns
void Process5VSns(PinSnsState_t *PState, uint32_t Len) {
    if(PState[0] == pssRising) App.SignalEvt(EVTMSK_USB_CONNECTED);
    else if(PState[0] == pssFalling) App.SignalEvt(EVTMSK_USB_DISCONNECTED);
}
