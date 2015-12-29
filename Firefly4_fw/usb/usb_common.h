/*
 * usb_common.h
 *
 *  Created on: 29 окт. 2015 г.
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

// Endpoint Sizes for Full-Speed devices
//#define EP0_SZ              64  // Control Endpoint must have a packet size of 64 bytes
//#define EP_INTERRUPT_SZ     8   // Max size is 64 bytes
//#define EP_BULK_SZ          64  // Max size is 64 bytes

#ifdef __cplusplus
extern "C" {
#endif
const USBDescriptor *GetDescriptor(USBDriver *usbp, uint8_t dtype, uint8_t dindex, uint16_t lang);
#ifdef __cplusplus
}
#endif

//extern const USBConfig UsbCfg;

class UsbCommon_t {
public:
    virtual void Init();
    virtual void Connect();// {
//        usbDisconnectBus(&USBDrv);
//        chThdSleepMilliseconds(1530);
//        usbStart(&USBDrv, &UsbCfg);
//        usbConnectBus(&USBDrv);
//    }
    void Disconnect() { usbDisconnectBus(&USBDrv); }
};

#endif /* USB_USB_COMMON_H_ */
