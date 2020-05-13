#pragma once 
#include "UsbComms.h"

typedef struct {
	u32 Interface;
	u32 IntEp;
	u32 BulkEp;
} UsbStreamPipe;

static inline bool UsbStreamSendHeader(UsbStreamPipe* pipe, const void* data)
{
	return UsbCommsTransfer(pipe->Interface, pipe->IntEp, UsbDirection_Write, data, 16, 3e+9) == 16;
}

static inline bool UsbStreamSendPayload(UsbStreamPipe* pipe, const void* data, u32 len)
{
	return UsbCommsTransfer(pipe->Interface, pipe->BulkEp, UsbDirection_Write, data, len, 1e+9) == len;
}

Result UsbSerialInitializeForStreaming(UsbStreamPipe* video, UsbStreamPipe* audio);
void UsbSerialExit();