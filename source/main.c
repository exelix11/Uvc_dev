#include <string.h>
#include <stdio.h>
#include <switch.h>

#include "USB/VideoDev.h"

int main(int argc, char **argv)
{
    consoleInit(NULL);

    printf("Hello people ! \n\n\n");

	UsbInterface Iface;

	Result rc = UsbVideoInitialize(&Iface);
	if (R_FAILED(rc)) {
		LOG("UsbVideoInitialize %x", rc);
		goto quitLoop;
	}

	LOGs("Initialized");

	while (appletMainLoop())
	{
		hidScanInput();
		if (hidKeysHeld(CONTROLLER_P1_AUTO) & KEY_PLUS) break;

		rc = UsbVideoHandleSetupPackets();
		if (R_FAILED(rc))
		{
			LOG("UsbVideoHandleSetupPackets %x", rc);
		}

		svcSleepThread(3E+9);
	}
  
quitLoop:
	LOGs("Start to quit");
	while(appletMainLoop())
    {
        hidScanInput();

        if (hidKeysHeld(CONTROLLER_P1_AUTO) & KEY_PLUS) break;

        consoleUpdate(NULL);
    }

	UsbCommsExit();
    consoleExit(NULL);
    return 0;
}
