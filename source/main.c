#include <string.h>
#include <stdio.h>
#include <switch.h>

#include <stdlib.h>
#include <stdatomic.h>

#include "USB/Serial.h"
#include "modes/modes.h"

VideoPacket alignas(0x1000) VPkt;
AudioPacket alignas(0x1000) APkt;

static atomic_bool Running = true;

void ReadVideo()
{
	VPkt.Header.Magic = 0xAAAAAAAA;
	VPkt.Header.Timestamp = rand();
	VPkt.Header.DataSize = 1 + rand() % sizeof(VPkt.Data);

	for (int i = 0; i < VPkt.Header.DataSize; i++)
		VPkt.Data[i] = rand();
}

void ReadAudio()
{
	APkt.Header.Magic = 0xBBBBBBBB;
	APkt.Header.Timestamp = rand();
	APkt.Header.DataSize = 666;

	for (int i = 0; i < APkt.Header.DataSize; i++)
		APkt.Data[i] = rand();
}

UsbStreamPipe video, audio;
void VideoLoop(void* a) 
{
	while (Running)
	{
		ReadVideo();

		if (!UsbStreamSendHeader(&video, &VPkt.Header))
		{
			svcSleepThread(1e+5);
			continue;
		}

		if (!UsbStreamSendPayload(&video, VPkt.Data, VPkt.Header.DataSize))
		{
			svcSleepThread(1e+5);
			continue;
		}
	}
	threadExit();
}

void AudioLoop(void* a) 
{
	while (Running)
	{
		ReadAudio();

		if (!UsbStreamSendHeader(&audio, &APkt.Header))
		{
			svcSleepThread(1e+5);
			continue;
		}

		if (!UsbStreamSendPayload(&audio, APkt.Data, APkt.Header.DataSize))
		{
			svcSleepThread(1e+5);
			continue;
		}
	}
	threadExit();
}

Result LaunchThread(Thread* t, ThreadFunc f)
{
	Result rc = threadCreate(t, f, NULL, NULL, 0x2000, 0x2C, -2);
	if (R_FAILED(rc)) return rc;
	rc = threadStart(t);
	if (R_FAILED(rc)) return rc;

	return 0;
}

int main(int argc, char **argv)
{
    consoleInit(NULL);

	printf("Hello people ! \n\n\n");
	
	Result rc = UsbSerialInitializeForStreaming(&video, &audio);
	if (R_FAILED(rc)) {
		LOG("UsbSerialInitializeForVideo %x", rc);
		goto quitLoop;
	}

	LOGs("Initialized");

	Thread v, a;

	rc = LaunchThread(&v, VideoLoop);
	if (R_FAILED(rc))
	{
		LOG("VIDEO launch %x", rc);
		goto quitLoop;
	}

	rc = LaunchThread(&a, AudioLoop);
	if (R_FAILED(rc))
	{
		LOG("AUDIO launch %x", rc);
		goto quitLoop;
	}

quitLoop:
	LOGs("Start to quit");
	while(appletMainLoop())
    {
        hidScanInput();

        if (hidKeysHeld(CONTROLLER_P1_AUTO) & KEY_PLUS) break;

        consoleUpdate(NULL);
    }

	LOGs("quitting...");

	Running = false;

	threadWaitForExit(&v);
	threadWaitForExit(&a);

	UsbSerialExit();
    consoleExit(NULL);
    return 0;
}
