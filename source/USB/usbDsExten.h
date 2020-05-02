#pragma once
#include <switch.h>

struct usb_setup_packet
{
	u8 bmRequestType;
	u8 bRequest;
	u16 wValue;
	u16 wIndex;
	u16 wLength;
} PACKED;

Result usbDsInterface_GetSetupEvent(UsbDsInterface* interface, Event* out);
Result usbDsInterface_GetCtrlOutCompletionEvent(UsbDsInterface* interface, Event* out);
//Result usbDsSetUsbDeviceDescriptorEx(UsbDeviceSpeed speed, const void* descriptor, u32 length);