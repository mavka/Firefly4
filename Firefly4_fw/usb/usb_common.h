/*
 * usb_common.h
 *
 *  Created on: 29 ���. 2015 �.
 *      Author: Kreyl
 */

#ifndef USB_USB_COMMON_H_
#define USB_USB_COMMON_H_

#include "board.h"
#include "kl_lib.h"

struct SetupPkt_t {
    uint8_t bmRequestType;
    uint8_t bRequest;
    union {
    	uint16_t wValue;
    	struct {
    		uint8_t wValueLSB;
    		uint8_t wValueMSB;
    	};
    };
    uint16_t wIndex;
    uint16_t wLength;
};

extern const USBConfig UsbCfg;

class UsbCommon_t {
public:
    void Init() {
#ifdef STM32F20XX
        PinSetupAlterFunc(USB_GPIO, USB_DM_PIN, omOpenDrain, pudNone, AF10);
        PinSetupAlterFunc(USB_GPIO, USB_DP_PIN, omOpenDrain, pudNone, AF10);
#endif
    }
    void Connect() {
        usbDisconnectBus(&USBDrv);
        chThdSleepMilliseconds(1530);
        usbStart(&USBDrv, &UsbCfg);
        usbConnectBus(&USBDrv);
    }
    void Disconnect() { usbDisconnectBus(&USBDrv); }
};

#endif /* USB_USB_COMMON_H_ */
