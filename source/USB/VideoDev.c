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

#define INTERFACE_CTRL_ID		0
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

static void ClearState()
{
#define clearVal(x) memset(&x, 0, sizeof(x))
	clearVal(ControlInterface);
	clearVal(StreamingInterface);
	clearVal(StreamingEndpoint);
	clearVal(VCIfaceDescriptor);
	clearVal(VSIfaceDescriptor);
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

	//ControlStream->interface = CONTROL_INTERFACE;
	//ControlStream->ReadEP = 0;
	//Control requests are being handled in this file

	VideoStream->interface = STREAM_INTERFACE;
	VideoStream->WriteEP = 0;

	UsbInterfaceDesc interfaces[2] = {0};

	interfaces[CONTROL_INTERFACE].interface_desc = &ControlInterface;
	interfaces[CONTROL_INTERFACE].string_descriptor = "Control";
	interfaces[CONTROL_INTERFACE].ExtraDescriptors.Addr = &VCIfaceDescriptor;
	interfaces[CONTROL_INTERFACE].ExtraDescriptors.Len = sizeof(VCIfaceDescriptor);
	interfaces[CONTROL_INTERFACE].ConfigDescriptors.Addr = &assoc_descriptor;
	interfaces[CONTROL_INTERFACE].ConfigDescriptors.Len = sizeof(assoc_descriptor);

	interfaces[STREAM_INTERFACE].interface_desc = &StreamingInterface;
	interfaces[STREAM_INTERFACE].endpoint_desc[0] = &StreamingEndpoint;
	interfaces[STREAM_INTERFACE].string_descriptor = "Stream";
	interfaces[STREAM_INTERFACE].ExtraDescriptors.Addr = &VSIfaceDescriptor;
	interfaces[STREAM_INTERFACE].ExtraDescriptors.Len = sizeof(VSIfaceDescriptor);

	Result rc = UsbCommsInitialize(&device_descriptor, 2, interfaces);
	if (R_FAILED(rc))
	{
		LOG("UsbCommsInitialize %x", rc);
		return rc;
	}

	UsbDsCtrl = UsbCommsGetInterface(CONTROL_INTERFACE);
	UsbDsVideo = UsbCommsGetInterface(STREAM_INTERFACE);

	return rc;
}

#define R_FAILED_LOG_RETURN(x, act) do { if (R_FAILED(x)) { LOG("%x @ %u", x, __LINE__); act; return rc; } } while (0)

u8 alignas(0x1000) ControlBuffer[0x1000];
Result UsbVideoHandleSetupPackets()
{
	u32 id;
	Event evt;

	Result rc = usbDsInterface_GetCtrlOutCompletionEvent(UsbDsCtrl, &evt);
	R_FAILED_LOG_RETURN(rc, (void)0);
	
	rc = usbDsInterface_CtrlOutPostBufferAsync(UsbDsCtrl, ControlBuffer, sizeof(ControlBuffer), &id);
	R_FAILED_LOG_RETURN(rc, eventClose(&evt));
	
	rc = eventWait(&evt, 5E+9);
	R_FAILED_LOG_RETURN(rc, eventClose(&evt));

	for (int i = 0; i < 0x100; i++)
		printf("%.2x ", ControlBuffer[i]);

	LOGs("\nDone");
	eventClose(&evt);
	return 0;
}
