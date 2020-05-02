#pragma once 
#include "UsbComms.h"

Result UsbVideoInitialize(UsbInterface* VideoStream);
Result UsbVideoHandleSetupPackets();