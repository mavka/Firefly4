/*
 * SnsPins.h
 *
 *  Created on: 17 ���. 2015 �.
 *      Author: Kreyl
 */

/* ================ Documentation =================
 * There are several (may be 1) groups of sensors (say, buttons and USB connection).
 * There is GPIO and Pin data for every sensor.
 *
 */

#ifndef PINSNSSETTINGS_H_
#define PINSNSSETTINGS_H_

#include "ch.h"
#include "hal.h"
#include <kl_lib.h>
#include "board.h"

#include "main.h" // App.thd here
#include "evt_mask.h"

#define SIMPLESENSORS_ENABLED   TRUE
#define SNS_POLL_PERIOD_MS      72

#if SIMPLESENSORS_ENABLED
enum PinSnsState_t {pssLo, pssHi, pssRising, pssFalling};
typedef void (*ftVoidPSnsStLen)(PinSnsState_t *PState, uint32_t Len);

// Single pin setup data
struct PinSns_t {
    GPIO_TypeDef *PGpio;
    uint16_t Pin;
    PinPullUpDown_t Pud;
    ftVoidPSnsStLen Postprocessor;
    void Init() const { PinSetupIn(PGpio, Pin, Pud); }
    void Off()  const { PinSetupAnalog(PGpio, Pin);  }
    bool IsHi() const { return PinIsSet(PGpio, Pin); }
};

// ================================= Settings ==================================
// Button handler
extern void Process5VSns(PinSnsState_t *PState, uint32_t Len);

const PinSns_t PinSns[] = {
        // 5V sns
        {SNS_5V_GPIO, SNS_5V_PIN, pudPullDown, Process5VSns},
};
#define PIN_SNS_CNT     countof(PinSns)

#endif  // if enabled

#endif /* PINSNSSETTINGS_H_ */
