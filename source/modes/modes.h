#pragma once
#include <switch.h>
#include "defines.h"
#include <stdint.h>
#include <stdbool.h>

#define VbufSz 0x32000
#define AbufSz 0x1000
#define AMaxBatch 4

typedef struct {
	u32 Magic;
	u32 DataSize;
	u64 Timestamp; //Note: timestamps are in usecs
} PacketHeader;

_Static_assert(sizeof(PacketHeader) == 16); //Ensure no padding, PACKED triggers a warning

typedef struct {
	u8 Data[VbufSz];
	PacketHeader Header;
} VideoPacket;

_Static_assert(sizeof(VideoPacket) == sizeof(PacketHeader) + VbufSz);

typedef struct {
	u8 Data[AbufSz * AMaxBatch];
	PacketHeader Header;
} AudioPacket;

_Static_assert(sizeof(AudioPacket) == sizeof(PacketHeader) + AbufSz * AMaxBatch);

extern VideoPacket VPkt;
extern AudioPacket APkt;