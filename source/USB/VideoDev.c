#include <string.h>

#include "VideoDev.h"
#include "uvc.h"
#include "../modes/modes.h"
#include "usbDsExten.h"

static Event VideoSetupEvent, ControlSetupEvent;
static UsbDsInterface *UsbDsVideo, *UsbDsCtrl;

#define FPS_TO_INTERVAL(fps)			((1E9 / 100) / (fps))

#define CONTROL_INTERFACE 		0
#define STREAM_INTERFACE		1
#define WORKAROUND_INTERFACE	2

#define INPUT_TERMINAL_ID		1
#define OUTPUT_TERMINAL_ID		2

#define UVC_INTERFACE_ASSOCIATION_DESCRIPTOR_TYPE	0x0B

DECLARE_UVC_HEADER_DESCRIPTOR(1);

static struct uvc_interface_association_descriptor assoc_descriptor;

typedef struct PACKED {
	struct UVC_HEADER_DESCRIPTOR(1) header_descriptor;
	struct uvc_input_terminal_descriptor input_terminal_descriptor;
	struct uvc_output_terminal_descriptor output_terminal_descriptor;
} VideoControlInterfaceDescriptor;

static VideoControlInterfaceDescriptor VCIfaceDescriptor;

DECLARE_UVC_INPUT_HEADER_DESCRIPTOR(1, 1);
DECLARE_UVC_FRAME_H264(1);

typedef struct PACKED {
	struct UVC_INPUT_HEADER_DESCRIPTOR(1, 1) input_header_descriptor;
	struct uvc_format_h264 h264_format;
	struct UVC_FRAME_H264(1) h264_frame;
} VideoStreamingInterfaceDescriptor;

static VideoStreamingInterfaceDescriptor VSIfaceDescriptor;

static struct usb_interface_descriptor ControlInterface;
static struct usb_interface_descriptor StreamingInterface;

//This is needed because there seems to be a limit on the descriptor length for a single interface and the streaming one doesn't cut it by 8 bytes.
//So this will append the VS descriptor before its own so hosts will interpet it as being part of the previous interface
static struct usb_interface_descriptor WorkaroundInterface;

static struct usb_endpoint_descriptor StreamingEndpoint;

static const struct uvc_streaming_control ControlSettings =
{
	.bmHint = 0,
	.bFormatIndex = 0,
	.bFrameIndex = 0,
	.dwFrameInterval = FPS_TO_INTERVAL(30),
	.wKeyFrameRate = 0,
	.wPFrameRate = 0,
	.wCompQuality = 0,
	.wCompWindowSize = 0,
	.wDelay = 0,
	.dwMaxVideoFrameSize = VbufSz,
	.dwMaxPayloadTransferSize = VbufSz + sizeof(struct uvc_payload_header_h264),
	.dwClockFrequency = 90000, //90 Khz
	.bmFramingInfo = 0,
	.bPreferedVersion = 0,
	.bMinVersion = 0,
	.bMaxVersion = 0,
};

//Todo: make a proper exist function
void ClearState()
{
#define clearVal(x) memset(&x, 0, sizeof(x))
	clearVal(ControlInterface);
	clearVal(StreamingInterface);
	clearVal(StreamingEndpoint);
	clearVal(VCIfaceDescriptor);
	clearVal(VSIfaceDescriptor);
	clearVal(WorkaroundInterface);
#undef clearVal
	if (eventActive(&VideoSetupEvent))
		eventClose(&VideoSetupEvent);
	if (eventActive(&ControlSetupEvent))
		eventClose(&ControlSetupEvent);
}

Result UsbVideoInitialize(UsbInterface* VideoStream)
{
	ClearState();

	struct usb_device_descriptor device_descriptor = {
		.bLength = USB_DT_DEVICE_SIZE,
		.bDescriptorType = USB_DT_DEVICE,
		.bcdUSB = 0x0200,
		.bDeviceClass = 0xEF,
		.bDeviceSubClass = 2,
		.bDeviceProtocol = 1,
		.bMaxPacketSize0 = 0x40,
		.idVendor = 0x057e,
		.idProduct = 0x3008,
		.bcdDevice = 0x0100,
		.bNumConfigurations = 0x01
	};

	// 3.6 Interface Association Descriptor 
	assoc_descriptor = (struct uvc_interface_association_descriptor){
		.bLength = sizeof(assoc_descriptor),
		.bDescriptorType = UVC_INTERFACE_ASSOCIATION_DESCRIPTOR_TYPE,
		.bFirstInterface = CONTROL_INTERFACE,
		.bInterfaceCount = 2,
		.bFunctionClass = UVC_CC_VIDEO,
		.bFunctionSubClass = UVC_SC_VIDEO_INTERFACE_COLLECTION,
		.bFunctionProtocol = UVC_PC_PROTOCOL_UNDEFINED,
		.iFunction = 4 //We know beforehand this is going to be 4, not the best way but works for me
	};

	// 3.7.1 Standard VC Interface Descriptor
	ControlInterface = (struct usb_interface_descriptor){
		.bLength = USB_DT_INTERFACE_SIZE,
		.bDescriptorType = USB_DT_INTERFACE,
		.bInterfaceNumber = CONTROL_INTERFACE,
		.bNumEndpoints = 0,
		.bInterfaceClass = USB_CLASS_VIDEO,
		.bInterfaceSubClass = UVC_SC_VIDEOCONTROL,
		.bInterfaceProtocol = UVC_PC_PROTOCOL_UNDEFINED,
		.iInterface = 0,
	};

	// 3.7.2 Class-Specific VC Interface Descriptor
	VCIfaceDescriptor = (VideoControlInterfaceDescriptor){
		.header_descriptor = {
			.bLength = sizeof(VCIfaceDescriptor.header_descriptor),
			.bDescriptorType = USB_DT_CS_INTERFACE,
			.bDescriptorSubType = UVC_VC_HEADER,
			.bcdUVC = 0x0150,
			.wTotalLength = sizeof(VCIfaceDescriptor),
			.dwClockFrequency = 90000,// 90Khz, previously: 48000000,
			.bInCollection = 1,
			.baInterfaceNr = {STREAM_INTERFACE},
		},		
		// 3.7.2.1 Input Terminal Descriptor
		.input_terminal_descriptor = {
			.bLength = sizeof(VCIfaceDescriptor.input_terminal_descriptor),
			.bDescriptorType = USB_DT_CS_INTERFACE,
			.bDescriptorSubType = UVC_VC_INPUT_TERMINAL,
			.bTerminalID = INPUT_TERMINAL_ID,
			.wTerminalType = UVC_ITT_VENDOR_SPECIFIC,
			.bAssocTerminal = 0,
			.iTerminal = 0
		},
		.output_terminal_descriptor = {
			.bLength = sizeof(VCIfaceDescriptor.output_terminal_descriptor),
			.bDescriptorType = USB_DT_CS_INTERFACE,
			.bDescriptorSubType = UVC_VC_OUTPUT_TERMINAL,
			.bTerminalID = OUTPUT_TERMINAL_ID,
			.wTerminalType = UVC_TT_STREAMING,
			.bAssocTerminal = 0,
			.bSourceID = INPUT_TERMINAL_ID,
			.iTerminal = 0,
		},
	};

	// 3.8.1VC Control Endpoint Descriptors
	// We only use Ep 0 and it doesn't reed a custom descriptor

	// 3.9.1 Standard VS Interface Descriptor
	StreamingInterface = (struct usb_interface_descriptor){
		.bLength = USB_DT_INTERFACE_SIZE,
		.bDescriptorType = USB_DT_INTERFACE,
		.bInterfaceNumber = STREAM_INTERFACE,
		.bNumEndpoints = 1,
		.bInterfaceClass = USB_CLASS_VIDEO,
		.bInterfaceSubClass = UVC_SC_VIDEOSTREAMING,
		.bInterfaceProtocol = UVC_PC_PROTOCOL_UNDEFINED,
	};

	// 3.9.2 Class-Specific VS Interface Descriptors
	VSIfaceDescriptor = (VideoStreamingInterfaceDescriptor){
		.input_header_descriptor = {
			.bLength = sizeof(VSIfaceDescriptor.input_header_descriptor),
			.bDescriptorType = USB_DT_CS_INTERFACE,
			.bDescriptorSubType = UVC_VS_INPUT_HEADER,
			.bNumFormats = 1,
			.wTotalLength = sizeof(VSIfaceDescriptor),
			.bEndpointAddress = 0x81,
			.bmInfo = 0,
			.bTerminalLink = OUTPUT_TERMINAL_ID,
			.bStillCaptureMethod = 0,
			.bTriggerSupport = 0,
			.bTriggerUsage = 0,
			.bControlSize = 1,
			.bmaControls = {{0}},
		},
		// 3.1.1 H.264 Video Format Descriptor (USB_Video_Payload_H264_1.5)
		.h264_format = {
			.bLength = sizeof(VSIfaceDescriptor.h264_format),
			.bDescriptorType = USB_DT_CS_INTERFACE,
			.bDescriptorSubType = UVC_VS_FORMAT_H264,
			.bFormatIndex = 0,
			.bNumFrameDescriptors = 1,
			.bDefaultFrameIndex = 0,
			.bMaxCodecConfigDelay = 1,
			.bmSupportedSliceModes = 0, //todo (?)
			.bmSupportedSyncFrameTypes = 0, //todo (?)
			.bResolutionScaling = 0,
			.Reserved1 = 0,
			.bmSupportedRateControlModes = 0
		},
		// 3.1.2 H.264Video Frame Descriptors (USB_Video_Payload_H264_1.5)
		.h264_frame = {
			.bLength = sizeof(VSIfaceDescriptor.h264_frame),
			.bDescriptorType = USB_DT_CS_INTERFACE,
			.bDescriptorSubType = UVC_VS_FRAME_H264,
			.bFrameIndex = 0,
			.wWidth = 1280,
			.wHeight = 720,
			.wSARwidth = 1,
			.wSARheight = 1,
			.wProfile = 0x640c, //First two bytes of sps
			.bLevelIDC = 32, //from sps
			.wConstrainedToolset = 0,
			.bmSupportedUsages = 0x70003, //todo(?) 
			.bmCapabilities = 0x47, //Constant frame rate
			.bmSVCCapabilities = 0,	// todo(?)
			.bmMVCCapabilities = 0, // todo(?)
			.dwMinBitRate = 8,
			.dwMaxBitRate = 0x61A800, //800 KB/s
			.dwDefaultFrameInterval = FPS_TO_INTERVAL(30),
			.bNumFrameIntervals = 1,
			.dwFrameInterval = { FPS_TO_INTERVAL(30) }
		}
	};

	// 3.10.1.2 Standard VS Bulk Video Data Endpoint Descriptor
	StreamingEndpoint = (struct usb_endpoint_descriptor){
		.bLength = USB_DT_ENDPOINT_SIZE,
		.bDescriptorType = USB_DT_ENDPOINT,
		.bEndpointAddress = USB_ENDPOINT_IN | 1,
		.bmAttributes = USB_TRANSFER_TYPE_BULK,
		.wMaxPacketSize = 0x200,
		.bInterval = 0
	};

	WorkaroundInterface = (struct usb_interface_descriptor){
		.bLength = USB_DT_INTERFACE_SIZE,
		.bDescriptorType = USB_DT_INTERFACE,
		.bInterfaceNumber = WORKAROUND_INTERFACE,
		.bNumEndpoints = 0,
		.bInterfaceClass = USB_CLASS_VENDOR_SPEC,
		.bInterfaceSubClass = 0,
		.bInterfaceProtocol = 0,
	};

	//ControlStream->interface = CONTROL_INTERFACE;
	//ControlStream->ReadEP = 0;
	//Control requests are being handled in this file

	VideoStream->interface = STREAM_INTERFACE;
	VideoStream->WriteEP = 0;

	UsbInterfaceDesc interfaces[3] = {0};

	interfaces[CONTROL_INTERFACE].interface_desc = &ControlInterface;
	interfaces[CONTROL_INTERFACE].string_descriptor = "Control";
	interfaces[CONTROL_INTERFACE].ExtraDescriptors.Addr = &VCIfaceDescriptor;
	interfaces[CONTROL_INTERFACE].ExtraDescriptors.Len = sizeof(VCIfaceDescriptor);
	interfaces[CONTROL_INTERFACE].ConfigDescriptors.Addr = &assoc_descriptor;
	interfaces[CONTROL_INTERFACE].ConfigDescriptors.Len = sizeof(assoc_descriptor);

	interfaces[STREAM_INTERFACE].interface_desc = &StreamingInterface;
	interfaces[STREAM_INTERFACE].endpoint_desc[0] = &StreamingEndpoint;
	interfaces[STREAM_INTERFACE].string_descriptor = "Stream";

	interfaces[WORKAROUND_INTERFACE].interface_desc = &WorkaroundInterface;
	interfaces[WORKAROUND_INTERFACE].ConfigDescriptors.Addr = &VSIfaceDescriptor;
	interfaces[WORKAROUND_INTERFACE].ConfigDescriptors.Len = sizeof(VSIfaceDescriptor);

	Result rc = UsbCommsInitialize(&device_descriptor, 3, interfaces);
	if (R_FAILED(rc))
	{
		LOG("UsbCommsInitialize %x", rc);
		return rc;
	}

	UsbDsCtrl = UsbCommsGetInterface(CONTROL_INTERFACE);
	UsbDsVideo = UsbCommsGetInterface(STREAM_INTERFACE);

	rc = usbDsInterface_GetSetupEvent(UsbDsCtrl, &ControlSetupEvent);
	rc = usbDsInterface_GetSetupEvent(UsbDsVideo, &VideoSetupEvent);

	return rc;
}

#define R_FAILED_RETURN(x) do { if (R_FAILED(x)) { LOG("%x @ %u", x, __LINE__); return rc; } } while (0)
#define R_FAILED_RETURN_ACT(x, a) do { if (R_FAILED(x)) { LOG("%x @ %u", x, __LINE__); a; return rc; } } while (0)

struct libusb_control_setup {
	u8  bmRequestType;
	u8  bRequest;
	u16 wValue;
	u16 wIndex;
	u16 wLength;
};

enum usb_control_type {
	USB_CTRLTYPE_TYPE_STANDARD = 0X00,
	USB_CTRLTYPE_TYPE_CLASS = 0X20,
	USB_CTRLTYPE_TYPE_VENDOR = 0X40,
	USB_CTRLTYPE_TYPE_RESERVED = 0X60,
	USB_CTRLTYPE_TYPE_MASK = 0X60
};

enum usb_control_recipient {
	USB_CTRLTYPE_REC_DEVICE = 0X00,
	USB_CTRLTYPE_REC_INTERFACE = 0X01,
	USB_CTRLTYPE_REC_ENDPOINT = 0X02,
	USB_CTRLTYPE_REC_OTHER = 0X03,
	USB_CTRLTYPE_REC_MASK = 0X1F
};

static u8 alignas(0x1000) usbbuf[64];
static Result ep0CtrlSend(const void* s, u32 len)
{
	memset(usbbuf, 0, sizeof(usbbuf));
	memcpy(usbbuf, s, len);

	//this seems to fail with 0x25c8c and after this usbds seems to break completely until console reboot
	u32 id;
	Result rc = usbDsInterface_CtrlInPostBufferAsync(UsbDsCtrl, &usbbuf, sizeof(usbbuf), &id);
	R_FAILED_RETURN(rc);
	LOG("urbid : %x", id);

	rc = eventWait(&UsbDsCtrl->CtrlInCompletionEvent, 5e+9);
	R_FAILED_RETURN(rc);

	UsbDsReportData data;
	rc = usbDsInterface_GetCtrlInReportData(UsbDsCtrl, &data);
	R_FAILED_RETURN(rc);

	LOG("%d report entries:", data.report_count);
	for (int i = 0; i < data.report_count; i++)
	{
		UsbDsReportEntry e = data.report[i];
		LOG("id %x, requestedSize %x, transferredSize %x, urb_status %x", e.id, e.requestedSize, e.transferredSize, e.urb_status);
	}

	return rc;
}

static Result ep0ReceiveData() 
{	
	u32 id;
	Result rc = usbDsInterface_CtrlOutPostBufferAsync(UsbDsCtrl, usbbuf, sizeof(usbbuf), &id);
	R_FAILED_RETURN(rc);
	LOG("urbid : %x", id);

	rc = eventWait(&UsbDsCtrl->CtrlOutCompletionEvent, 5e+9);
	R_FAILED_RETURN(rc);

	UsbDsReportData data;
	rc = usbDsInterface_GetCtrlOutReportData(UsbDsCtrl, &data);
	R_FAILED_RETURN(rc);

	UsbDsReportEntry* correct;

	LOG("%d report entries:", data.report_count);
	for (int i = 0; i < data.report_count; i++)
	{
		UsbDsReportEntry e = data.report[i];
		LOG("id %x, requestedSize %x, transferredSize %x, urb_status %x", e.id, e.requestedSize, e.transferredSize, e.urb_status);
		if (e.id == id) correct = &e;
	}

	u32 sz = sizeof(usbbuf);
	if (correct && correct->transferredSize > 0 && correct->transferredSize <= sz) sz = correct->transferredSize;

	LOGs("data:");
	for (u32 i = 0; i < sz; i++)
		printf("%.2x ", usbbuf[i]);
	
	LOGs(":");

	return rc;
}

static void uvc_handle_video_streaming_req(const struct libusb_control_setup* req)
{
	LOG("  uvc_handle_video_streaming_req %x, %x\n", req->wValue, req->bRequest);

	switch (req->wValue >> 8) {
	case UVC_VS_PROBE_CONTROL:
		switch (req->bRequest) {
		case UVC_GET_INFO:
			break;
		case UVC_GET_LEN:
			break;
		case UVC_GET_MIN:
		case UVC_GET_MAX:
		case UVC_GET_DEF:
			LOG("Probe GET_DEF, bFormatIndex: %d, bmFramingInfo: %x\n",
				ControlSettings.bFormatIndex,
				ControlSettings.bmFramingInfo);
			ep0CtrlSend(&ControlSettings, sizeof(ControlSettings));
			break;
		case UVC_GET_CUR:
			LOG("Probe GET_CUR, bFormatIndex: %d, bmFramingInfo: %x\n",
				ControlSettings.bFormatIndex,
				ControlSettings.bmFramingInfo);
			ep0CtrlSend(&ControlSettings, sizeof(ControlSettings));
			break;
		case UVC_SET_CUR:
			ep0ReceiveData(req);
			break;
		}
		break;
	case UVC_VS_COMMIT_CONTROL:
		switch (req->bRequest) {
		case UVC_GET_INFO:
			break;
		case UVC_GET_LEN:
			break;
		case UVC_GET_CUR:
			ep0CtrlSend(&ControlSettings, sizeof(ControlSettings));
			break;
		case UVC_SET_CUR:
			ep0ReceiveData(req);
			break;
		}
		break;
	}
}

struct libusb_control_setup /*alignas(0x1000)*/ setupPacket;
static Result InternalUsbVideoHandleSetupPackets(UsbDsInterface* iface)
{
	memset(&setupPacket, 0xAA, sizeof(setupPacket));
	Result rc;

	rc = eventWait(&UsbDsCtrl->CtrlOutCompletionEvent, 2e+9);
	R_FAILED_RETURN(rc);

	//Result rc = eventWait(&VideoSetupEvent, 5E+9); //This never seems to fire
	//if (rc)
	//{
	//	printf("eventwait result : %x\n", rc);
	//}

	rc = usbDsInterface_GetSetupPacket(UsbDsCtrl, &setupPacket, sizeof(setupPacket));
	R_FAILED_RETURN(rc);

	printf("got setup: ");
	for (int i = 0; i < sizeof(setupPacket); i++)
		printf("%.2x ", ((u8*)&setupPacket)[i]);
	printf("\n");

	switch (setupPacket.bmRequestType) {
	case USB_ENDPOINT_IN | USB_CTRLTYPE_TYPE_CLASS | USB_CTRLTYPE_REC_INTERFACE: // 0xA1
	case USB_ENDPOINT_OUT | USB_CTRLTYPE_TYPE_CLASS | USB_CTRLTYPE_REC_INTERFACE: // 0x21
		switch (setupPacket.wIndex & 0xFF) {
		case CONTROL_INTERFACE:
			switch (setupPacket.wIndex >> 8) {
			case CONTROL_INTERFACE:
				LOGs("uvc_handle_interface_ctrl_req");
				break;
			case INPUT_TERMINAL_ID:
				LOGs("uvc_handle_input_terminal_req");
				break;
			case OUTPUT_TERMINAL_ID:
				LOGs("uvc_handle_output_terminal_req");
				break;
			}
			break;
		case STREAM_INTERFACE:
			uvc_handle_video_streaming_req(&setupPacket);
			break;
		}
		break;
	case USB_ENDPOINT_OUT | USB_CTRLTYPE_TYPE_STANDARD | USB_CTRLTYPE_REC_INTERFACE: // 0x01
		switch (setupPacket.bRequest) {
		case USB_REQUEST_SET_INTERFACE:
			LOGs("uvc_handle_set_interface");
			break;
		}
		break;
	case USB_ENDPOINT_OUT | USB_CTRLTYPE_TYPE_STANDARD | USB_CTRLTYPE_REC_ENDPOINT: // 0x02
		switch (setupPacket.bRequest) {
		case USB_REQUEST_CLEAR_FEATURE:
			LOGs("uvc_handle_clear_feature");
			break;
		}
		break;
	case USB_ENDPOINT_IN | USB_CTRLTYPE_TYPE_STANDARD | USB_CTRLTYPE_REC_DEVICE: // 0x80
		switch (setupPacket.wValue >> 8) {
		case 0x0A: /* USB_DT_DEBUG */
			break;
		}
	default:
		LOG("Unknown bmRequestType: 0x%02X\n", setupPacket.bmRequestType);
	}

	LOGs("Done");
	return 0;
}

Result UsbVideoHandleSetupPackets()
{
	return InternalUsbVideoHandleSetupPackets(UsbDsCtrl);
}