/*
 * main.h
 *
 *  Created on: 26 дек. 2015 г.
 *      Author: Kreyl
 */

#ifndef MAIN_H_
#define MAIN_H_

#include "ch.h"
#include "kl_lib.h"
#include "uart.h"
#include "evt_mask.h"
#include "board.h"

#define APP_NAME            "GestRec_f4"
#define APP_VERSION         __DATE__ " " __TIME__

#define SNS_LOW_THRESHOLD       999
#define SNS_HIGH_THRESHOLD      1710
#define SAMPLING_INTERVAL_MS    110
#define RESET_INTERVAL          2700

class App_t {
private:
    thread_t *PThread;
    TmrVirtual_t TmrReset;
    LowHigh_t Prev0 = Low, Prev1 = Low;
    LowHigh_t Normalize(uint32_t X, LowHigh_t PrevX);
    RiseFall_t DetectEdge(LowHigh_t X, LowHigh_t PrevX);
    uint32_t CntU=0, CntD=0;
    void ResetCounters() {
        CntU=0;
        CntD=0;
        TmrReset.Stop();
    }
    void ProcessValues(uint32_t Sns0, uint32_t Sns1);
public:
    TmrVirtual_t TmrSampling;
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


#endif /* MAIN_H_ */
