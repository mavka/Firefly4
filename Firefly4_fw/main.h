/*
 * main.h
 *
 *  Created on: 26 дек. 2015 г.
 *      Author: Kreyl
 */

#pragma once

#include "ch.h"
#include "kl_lib.h"
#include "uart.h"
#include "evt_mask.h"
#include "board.h"

#define APP_NAME                "GestRec"
#define APP_VERSION             __DATE__ " " __TIME__

#define SNS_THRESHOLD           999
#define SAMPLING_INTERVAL_MS    11
#define RESET_INTERVAL          2700

class App_t {
private:
    thread_t *PThread;
    void ProcessValues(uint32_t Sns0, uint32_t Sns1);
public:
    TmrKL_t TmrSampling;
    // Eternal methods
    void InitThread() { PThread = chThdGetSelfX(); }
    void SignalEvt(eventmask_t Evt) {
        chSysLock();
        chEvtSignalI(PThread, Evt);
        chSysUnlock();
    }
    void SignalEvtI(eventmask_t Evt) { chEvtSignalI(PThread, Evt); }
    void OnCmd(Shell_t *PShell);
    // Inner use
    void ITask();
};

extern App_t App;
