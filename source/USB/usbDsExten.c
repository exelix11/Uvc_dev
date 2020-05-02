#include "usbDsExten.h"

Result usbDsInterface_GetSetupEvent(UsbDsInterface* interface, Event* out) {
	if (!interface->initialized) return MAKERESULT(Module_Libnx, LibnxError_NotInitialized);
	Handle event = INVALID_HANDLE;

	serviceAssumeDomain(&interface->s);
	Result rc = serviceDispatch(&interface->s, 1,
		.out_handle_attrs = { SfOutHandleAttr_HipcCopy },
		.out_handles = &event
	);

	if (R_SUCCEEDED(rc))
		eventLoadRemote(out, event, true);

	return rc;
}

Result usbDsInterface_GetCtrlOutCompletionEvent(UsbDsInterface* interface, Event* out) {
	if (!interface->initialized) return MAKERESULT(Module_Libnx, LibnxError_NotInitialized);
	Handle event = INVALID_HANDLE;

	serviceAssumeDomain(&interface->s);
	Result rc = serviceDispatch(&interface->s, 9,
		.out_handle_attrs = { SfOutHandleAttr_HipcCopy },
		.out_handles = &event
	);

	if (R_SUCCEEDED(rc))
		eventLoadRemote(out, event, true);

	return rc;
}

//Result usbDsSetUsbDeviceDescriptorEx(UsbDeviceSpeed speed, const void* descriptor, u32 length) {
//	if (hosversionBefore(5, 0, 0))
//		return MAKERESULT(Module_Libnx, LibnxError_IncompatSysVer);
//
//	u32 tmp = speed;
//	serviceAssumeDomain(&g_usbDsSrv);
//	return serviceDispatchIn(&g_usbDsSrv, 8, tmp,
//		.buffer_attrs = { SfBufferAttr_HipcMapAlias | SfBufferAttr_In },
//		.buffers = { { descriptor, length } },
//	);
//}