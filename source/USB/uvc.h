#ifndef UVC_H
#define UVC_H

#include <stdint.h>

#define __packed__ packed

typedef uint8_t __u8;
typedef uint16_t __u16;
typedef uint32_t __u32;

#define USB_TYPE_CLASS			(0x01 << 5)
#define USB_DT_CS_DEVICE		(USB_TYPE_CLASS | USB_DT_DEVICE)
#define USB_DT_CS_CONFIG		(USB_TYPE_CLASS | USB_DT_CONFIG)
#define USB_DT_CS_STRING		(USB_TYPE_CLASS | USB_DT_STRING)
#define USB_DT_CS_INTERFACE		(USB_TYPE_CLASS | USB_DT_INTERFACE)
#define USB_DT_CS_ENDPOINT		(USB_TYPE_CLASS | USB_DT_ENDPOINT)

/*
 * Copied from https://github.com/torvalds/linux/blob/master/include/uapi/linux/usb/video.h
 */

 /* --------------------------------------------------------------------------
  * UVC constants
  */

/* A.1. Video Interface Class Codes*/
#define UVC_CC_VIDEO			0x0E

/* A.2. Video Interface Subclass Codes */
#define UVC_SC_UNDEFINED				0x00
#define UVC_SC_VIDEOCONTROL				0x01
#define UVC_SC_VIDEOSTREAMING				0x02
#define UVC_SC_VIDEO_INTERFACE_COLLECTION		0x03

/* A.3. Video Interface Protocol Codes */
#define UVC_PC_PROTOCOL_UNDEFINED			0x00
#define UVC_PC_PROTOCOL_15				0x01

/* A.5. Video Class-Specific VC Interface Descriptor Subtypes */
#define UVC_VC_DESCRIPTOR_UNDEFINED			0x00
#define UVC_VC_HEADER					0x01
#define UVC_VC_INPUT_TERMINAL				0x02
#define UVC_VC_OUTPUT_TERMINAL				0x03
#define UVC_VC_SELECTOR_UNIT				0x04
#define UVC_VC_PROCESSING_UNIT				0x05
#define UVC_VC_EXTENSION_UNIT				0x06

/* A.6. Video Class-Specific VS Interface Descriptor Subtypes */
#define UVC_VS_UNDEFINED				0x00
#define UVC_VS_INPUT_HEADER				0x01
#define UVC_VS_OUTPUT_HEADER				0x02
#define UVC_VS_STILL_IMAGE_FRAME			0x03
#define UVC_VS_FORMAT_UNCOMPRESSED			0x04
#define UVC_VS_FRAME_UNCOMPRESSED			0x05
#define UVC_VS_FORMAT_MJPEG				0x06
#define UVC_VS_FRAME_MJPEG				0x07
#define UVC_VS_FORMAT_MPEG2TS				0x0a
#define UVC_VS_FORMAT_DV				0x0c
#define UVC_VS_COLORFORMAT				0x0d
#define UVC_VS_FORMAT_FRAME_BASED			0x10
#define UVC_VS_FRAME_FRAME_BASED			0x11
#define UVC_VS_FORMAT_STREAM_BASED			0x12

/* A.7. Video Class-Specific Endpoint Descriptor Subtypes */
#define UVC_EP_UNDEFINED				0x00
#define UVC_EP_GENERAL					0x01
#define UVC_EP_ENDPOINT					0x02
#define UVC_EP_INTERRUPT				0x03

/* A.8. Video Class-Specific Request Codes */
#define UVC_RC_UNDEFINED				0x00
#define UVC_SET_CUR					0x01
#define UVC_GET_CUR					0x81
#define UVC_GET_MIN					0x82
#define UVC_GET_MAX					0x83
#define UVC_GET_RES					0x84
#define UVC_GET_LEN					0x85
#define UVC_GET_INFO					0x86
#define UVC_GET_DEF					0x87

/* A.9.1. VideoControl Interface Control Selectors */
#define UVC_VC_CONTROL_UNDEFINED			0x00
#define UVC_VC_VIDEO_POWER_MODE_CONTROL			0x01
#define UVC_VC_REQUEST_ERROR_CODE_CONTROL		0x02

/* A.9.2. Terminal Control Selectors */
#define UVC_TE_CONTROL_UNDEFINED			0x00

/* A.9.3. Selector Unit Control Selectors */
#define UVC_SU_CONTROL_UNDEFINED			0x00
#define UVC_SU_INPUT_SELECT_CONTROL			0x01

/* A.9.4. Camera Terminal Control Selectors */
#define UVC_CT_CONTROL_UNDEFINED			0x00
#define UVC_CT_SCANNING_MODE_CONTROL			0x01
#define UVC_CT_AE_MODE_CONTROL				0x02
#define UVC_CT_AE_PRIORITY_CONTROL			0x03
#define UVC_CT_EXPOSURE_TIME_ABSOLUTE_CONTROL		0x04
#define UVC_CT_EXPOSURE_TIME_RELATIVE_CONTROL		0x05
#define UVC_CT_FOCUS_ABSOLUTE_CONTROL			0x06
#define UVC_CT_FOCUS_RELATIVE_CONTROL			0x07
#define UVC_CT_FOCUS_AUTO_CONTROL			0x08
#define UVC_CT_IRIS_ABSOLUTE_CONTROL			0x09
#define UVC_CT_IRIS_RELATIVE_CONTROL			0x0a
#define UVC_CT_ZOOM_ABSOLUTE_CONTROL			0x0b
#define UVC_CT_ZOOM_RELATIVE_CONTROL			0x0c
#define UVC_CT_PANTILT_ABSOLUTE_CONTROL			0x0d
#define UVC_CT_PANTILT_RELATIVE_CONTROL			0x0e
#define UVC_CT_ROLL_ABSOLUTE_CONTROL			0x0f
#define UVC_CT_ROLL_RELATIVE_CONTROL			0x10
#define UVC_CT_PRIVACY_CONTROL				0x11

/* A.9.5. Processing Unit Control Selectors */
#define UVC_PU_CONTROL_UNDEFINED			0x00
#define UVC_PU_BACKLIGHT_COMPENSATION_CONTROL		0x01
#define UVC_PU_BRIGHTNESS_CONTROL			0x02
#define UVC_PU_CONTRAST_CONTROL				0x03
#define UVC_PU_GAIN_CONTROL				0x04
#define UVC_PU_POWER_LINE_FREQUENCY_CONTROL		0x05
#define UVC_PU_HUE_CONTROL				0x06
#define UVC_PU_SATURATION_CONTROL			0x07
#define UVC_PU_SHARPNESS_CONTROL			0x08
#define UVC_PU_GAMMA_CONTROL				0x09
#define UVC_PU_WHITE_BALANCE_TEMPERATURE_CONTROL	0x0a
#define UVC_PU_WHITE_BALANCE_TEMPERATURE_AUTO_CONTROL	0x0b
#define UVC_PU_WHITE_BALANCE_COMPONENT_CONTROL		0x0c
#define UVC_PU_WHITE_BALANCE_COMPONENT_AUTO_CONTROL	0x0d
#define UVC_PU_DIGITAL_MULTIPLIER_CONTROL		0x0e
#define UVC_PU_DIGITAL_MULTIPLIER_LIMIT_CONTROL		0x0f
#define UVC_PU_HUE_AUTO_CONTROL				0x10
#define UVC_PU_ANALOG_VIDEO_STANDARD_CONTROL		0x11
#define UVC_PU_ANALOG_LOCK_STATUS_CONTROL		0x12

/* A.9.7. VideoStreaming Interface Control Selectors */
#define UVC_VS_CONTROL_UNDEFINED			0x00
#define UVC_VS_PROBE_CONTROL				0x01
#define UVC_VS_COMMIT_CONTROL				0x02
#define UVC_VS_STILL_PROBE_CONTROL			0x03
#define UVC_VS_STILL_COMMIT_CONTROL			0x04
#define UVC_VS_STILL_IMAGE_TRIGGER_CONTROL		0x05
#define UVC_VS_STREAM_ERROR_CODE_CONTROL		0x06
#define UVC_VS_GENERATE_KEY_FRAME_CONTROL		0x07
#define UVC_VS_UPDATE_FRAME_SEGMENT_CONTROL		0x08
#define UVC_VS_SYNC_DELAY_CONTROL			0x09

/* B.1. USB Terminal Types */
#define UVC_TT_VENDOR_SPECIFIC				0x0100
#define UVC_TT_STREAMING				0x0101

/* B.2. Input Terminal Types */
#define UVC_ITT_VENDOR_SPECIFIC				0x0200
#define UVC_ITT_CAMERA					0x0201
#define UVC_ITT_MEDIA_TRANSPORT_INPUT			0x0202

/* B.3. Output Terminal Types */
#define UVC_OTT_VENDOR_SPECIFIC				0x0300
#define UVC_OTT_DISPLAY					0x0301
#define UVC_OTT_MEDIA_TRANSPORT_OUTPUT			0x0302

/* B.4. External Terminal Types */
#define UVC_EXTERNAL_VENDOR_SPECIFIC			0x0400
#define UVC_COMPOSITE_CONNECTOR				0x0401
#define UVC_SVIDEO_CONNECTOR				0x0402
#define UVC_COMPONENT_CONNECTOR				0x0403

/* 2.4.2.2. Status Packet Type */
#define UVC_STATUS_TYPE_CONTROL				1
#define UVC_STATUS_TYPE_STREAMING			2

/* 2.4.3.3. Payload Header Information */
#define UVC_STREAM_EOH					(1 << 7)
#define UVC_STREAM_ERR					(1 << 6)
#define UVC_STREAM_STI					(1 << 5)
#define UVC_STREAM_RES					(1 << 4)
#define UVC_STREAM_SCR					(1 << 3)
#define UVC_STREAM_PTS					(1 << 2)
#define UVC_STREAM_EOF					(1 << 1)
#define UVC_STREAM_FID					(1 << 0)

/* 4.1.2. Control Capabilities */
#define UVC_CONTROL_CAP_GET				(1 << 0)
#define UVC_CONTROL_CAP_SET				(1 << 1)
#define UVC_CONTROL_CAP_DISABLED			(1 << 2)
#define UVC_CONTROL_CAP_AUTOUPDATE			(1 << 3)
#define UVC_CONTROL_CAP_ASYNCHRONOUS			(1 << 4)

/* ------------------------------------------------------------------------
 * UVC structures
 */

 /* All UVC descriptors have these 3 fields at the beginning */
struct uvc_descriptor_header {
	__u8  bLength;
	__u8  bDescriptorType;
	__u8  bDescriptorSubType;
} __attribute__((__packed__));

/* 3.6 Interface Association Descriptor */
struct uvc_interface_association_descriptor {
	__u8 bLength;
	__u8 bDescriptorType;
	__u8 bFirstInterface;
	__u8 bInterfaceCount;
	__u8 bFunctionClass;
	__u8 bFunctionSubClass;
	__u8 bFunctionProtocol;
	__u8 iFunction;
} __attribute__((__packed__));

/* 3.7.2. Video Control Interface Header Descriptor */
struct uvc_header_descriptor {
	__u8  bLength;
	__u8  bDescriptorType;
	__u8  bDescriptorSubType;
	__u16 bcdUVC;
	__u16 wTotalLength;
	__u32 dwClockFrequency;
	__u8  bInCollection;
	__u8  baInterfaceNr[];
} __attribute__((__packed__));

#define UVC_DT_HEADER_SIZE(n)				(12+(n))

#define UVC_HEADER_DESCRIPTOR(n) \
	uvc_header_descriptor_##n

#define DECLARE_UVC_HEADER_DESCRIPTOR(n)		\
struct UVC_HEADER_DESCRIPTOR(n) {			\
	__u8  bLength;					\
	__u8  bDescriptorType;				\
	__u8  bDescriptorSubType;			\
	__u16 bcdUVC;					\
	__u16 wTotalLength;				\
	__u32 dwClockFrequency;				\
	__u8  bInCollection;				\
	__u8  baInterfaceNr[n];				\
} __attribute__((__packed__))

/* 3.7.2.1. Input Terminal Descriptor */
struct uvc_input_terminal_descriptor {
	__u8  bLength;
	__u8  bDescriptorType;
	__u8  bDescriptorSubType;
	__u8  bTerminalID;
	__u16 wTerminalType;
	__u8  bAssocTerminal;
	__u8  iTerminal;
} __attribute__((__packed__));

#define UVC_DT_INPUT_TERMINAL_SIZE			8

/* 3.7.2.2. Output Terminal Descriptor */
struct uvc_output_terminal_descriptor {
	__u8  bLength;
	__u8  bDescriptorType;
	__u8  bDescriptorSubType;
	__u8  bTerminalID;
	__u16 wTerminalType;
	__u8  bAssocTerminal;
	__u8  bSourceID;
	__u8  iTerminal;
} __attribute__((__packed__));

#define UVC_DT_OUTPUT_TERMINAL_SIZE			9

/* 3.7.2.3. Camera Terminal Descriptor */
struct uvc_camera_terminal_descriptor {
	__u8  bLength;
	__u8  bDescriptorType;
	__u8  bDescriptorSubType;
	__u8  bTerminalID;
	__u16 wTerminalType;
	__u8  bAssocTerminal;
	__u8  iTerminal;
	__u16 wObjectiveFocalLengthMin;
	__u16 wObjectiveFocalLengthMax;
	__u16 wOcularFocalLength;
	__u8  bControlSize;
	__u8  bmControls[3];
} __attribute__((__packed__));

#define UVC_DT_CAMERA_TERMINAL_SIZE(n)			(15+(n))

/* 3.8.2.2. Video Control Interrupt Endpoint Descriptor */
struct uvc_control_endpoint_descriptor {
	__u8  bLength;
	__u8  bDescriptorType;
	__u8  bDescriptorSubType;
	__u16 wMaxTransferSize;
} __attribute__((__packed__));

#define UVC_DT_CONTROL_ENDPOINT_SIZE			5

/* 3.9.2.1. Input Header Descriptor */
struct uvc_input_header_descriptor {
	__u8  bLength;
	__u8  bDescriptorType;
	__u8  bDescriptorSubType;
	__u8  bNumFormats;
	__u16 wTotalLength;
	__u8  bEndpointAddress;
	__u8  bmInfo;
	__u8  bTerminalLink;
	__u8  bStillCaptureMethod;
	__u8  bTriggerSupport;
	__u8  bTriggerUsage;
	__u8  bControlSize;
	__u8  bmaControls[];
} __attribute__((__packed__));

#define UVC_DT_INPUT_HEADER_SIZE(n, p)			(13+(n*p))

#define UVC_INPUT_HEADER_DESCRIPTOR(n, p) \
	uvc_input_header_descriptor_##n_##p

#define DECLARE_UVC_INPUT_HEADER_DESCRIPTOR(n, p)	\
struct UVC_INPUT_HEADER_DESCRIPTOR(n, p) {		\
	__u8  bLength;					\
	__u8  bDescriptorType;				\
	__u8  bDescriptorSubType;			\
	__u8  bNumFormats;				\
	__u16 wTotalLength;				\
	__u8  bEndpointAddress;				\
	__u8  bmInfo;					\
	__u8  bTerminalLink;				\
	__u8  bStillCaptureMethod;			\
	__u8  bTriggerSupport;				\
	__u8  bTriggerUsage;				\
	__u8  bControlSize;				\
	__u8  bmaControls[p][n];			\
} __attribute__((__packed__))

/* 3.9.2.2. Output Header Descriptor */
struct uvc_output_header_descriptor {
	__u8  bLength;
	__u8  bDescriptorType;
	__u8  bDescriptorSubType;
	__u8  bNumFormats;
	__u16 wTotalLength;
	__u8  bEndpointAddress;
	__u8  bTerminalLink;
	__u8  bControlSize;
	__u8  bmaControls[];
} __attribute__((__packed__));

#define UVC_DT_OUTPUT_HEADER_SIZE(n, p)			(9+(n*p))

#define UVC_OUTPUT_HEADER_DESCRIPTOR(n, p) \
	uvc_output_header_descriptor_##n_##p

#define DECLARE_UVC_OUTPUT_HEADER_DESCRIPTOR(n, p)	\
struct UVC_OUTPUT_HEADER_DESCRIPTOR(n, p) {		\
	__u8  bLength;					\
	__u8  bDescriptorType;				\
	__u8  bDescriptorSubType;			\
	__u8  bNumFormats;				\
	__u16 wTotalLength;				\
	__u8  bEndpointAddress;				\
	__u8  bTerminalLink;				\
	__u8  bControlSize;				\
	__u8  bmaControls[p][n];			\
} __attribute__ ((__packed__))

/* 4.3.1.1. Video Probe and Commit Controls */
struct uvc_streaming_control {
	__u16 bmHint;
	__u8  bFormatIndex;
	__u8  bFrameIndex;
	__u32 dwFrameInterval;
	__u16 wKeyFrameRate;
	__u16 wPFrameRate;
	__u16 wCompQuality;
	__u16 wCompWindowSize;
	__u16 wDelay;
	__u32 dwMaxVideoFrameSize;
	__u32 dwMaxPayloadTransferSize;
	__u32 dwClockFrequency;
	__u8  bmFramingInfo;
	__u8  bPreferedVersion;
	__u8  bMinVersion;
	__u8  bMaxVersion;
} __attribute__((__packed__));

#define UVC_VS_FORMAT_H264	0x13
#define UVC_VS_FRAME_H264	0x14
#define UVC_VS_FORMAT_H264_SIMULCAST	0x15

struct uvc_format_h264 {
	__u8  bLength;
	__u8  bDescriptorType;
	__u8  bDescriptorSubType;
	__u8  bFormatIndex;
	__u8  bNumFrameDescriptors;
	__u8  bDefaultFrameIndex;
	__u8  bMaxCodecConfigDelay;
	__u8  bmSupportedSliceModes;
	__u8  bmSupportedSyncFrameTypes;
	__u8  bResolutionScaling;
	__u8  Reserved1;
	__u8  bmSupportedRateControlModes;
	__u16 wMaxMBperSecOneResNoScalability;
	__u16 wMaxMBperSecTwoResNoScalability;
	__u16 wMaxMBperSecThreeResNoScalability;
	__u16 wMaxMBperSecFourResNoScalability;
	__u16 wMaxMBperSecOneResTemporalScalability;
	__u16 wMaxMBperSecTwoResTemporalScalability;
	__u16 wMaxMBperSecThreeResTemporalScalability;
	__u16 wMaxMBperSecFourResTemporalScalability;
	__u16 wMaxMBperSecOneResTemporalQualityScalability;
	__u16 wMaxMBperSecTwoResTemporalQualityScalability;
	__u16 wMaxMBperSecThreeResTemporalQualityScalability;
	__u16 wMaxMBperSecFourResTemporalQualityScalability;
	__u16 wMaxMBperSecOneResTemporalSpatialScalability;
	__u16 wMaxMBperSecTwoResTemporalSpatialScalability;
	__u16 wMaxMBperSecThreeResTemporalSpatialScalability;
	__u16 wMaxMBperSecFourResTemporalSpatialScalability;
	__u16 wMaxMBperSecOneResFullScalability;
	__u16 wMaxMBperSecTwoResFullScalability;
	__u16 wMaxMBperSecThreeResFullScalability;
	__u16 wMaxMBperSecFourResFullScalability;
} __attribute__((__packed__));


/* H264 Payload - 3.1.2. H264 Video Frame Descriptor */
struct uvc_frame_h264 {
	__u8  bLength;
	__u8  bDescriptorType;
	__u8  bDescriptorSubType;
	__u8  bFrameIndex;
	__u16 wWidth;
	__u16 wHeight;
	__u16 wSARwidth;
	__u16 wSARheight;
	__u16 wProfile;
	__u8  bLevelIDC;
	__u16 wConstrainedToolset;
	__u32 bmSupportedUsages;
	__u16 bmCapabilities;
	__u32 bmSVCCapabilities;
	__u32 bmMVCCapabilities;
	__u32 dwMinBitRate;
	__u32 dwMaxBitRate;
	__u32 dwDefaultFrameInterval;
	__u8  bNumFrameIntervals;
	__u32 dwFrameInterval[];
} __attribute__((__packed__));

#define UVC_DT_FRAME_H264_SIZE(n)	(44+4*(n))

#define UVC_FRAME_H264(n) \
	uvc_frame_h264_##n

#define DECLARE_UVC_FRAME_H264(n)		\
struct UVC_FRAME_H264(n) {				\
		__u8  bLength;					\
		__u8  bDescriptorType;			\
		__u8  bDescriptorSubType;		\
		__u8  bFrameIndex;				\
		__u16 wWidth;					\
		__u16 wHeight;					\
		__u16 wSARwidth;				\
		__u16 wSARheight;				\
		__u16 wProfile;					\
		__u8  bLevelIDC;				\
		__u16 wConstrainedToolset;		\
		__u32 bmSupportedUsages;		\
		__u16 bmCapabilities;			\
		__u32 bmSVCCapabilities;		\
		__u32 bmMVCCapabilities;		\
		__u32 dwMinBitRate;				\
		__u32 dwMaxBitRate;				\
		__u32 dwDefaultFrameInterval;	\
		__u8  bNumFrameIntervals;		\
		__u32 dwFrameInterval[n];		\
} __attribute__((packed))

// 2.2 Stream Header (USB_Video_Payload_H264_1.5)
struct uvc_payload_header_h264
{
	__u8 bHeaderLength;
	__u8 bmHeaderInfo;
	__u32 dwPresentationTime;
	__u8 scrSourceClock[6];
	__u16 wLayerOrViewID;
} __attribute__((packed));

/*
 * Copied from https://github.com/torvalds/linux/blob/master/drivers/media/usb/uvc/uvcvideo.h
 */

 /* ------------------------------------------------------------------------
  * GUIDs
  */
#define UVC_GUID_UVC_CAMERA \
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01}
#define UVC_GUID_UVC_OUTPUT \
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02}
#define UVC_GUID_UVC_MEDIA_TRANSPORT_INPUT \
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03}
#define UVC_GUID_UVC_PROCESSING \
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01}
#define UVC_GUID_UVC_SELECTOR \
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x02}

#define UVC_GUID_FORMAT_H264 \
	{ 'H',  '2',  '6',  '4', 0x00, 0x00, 0x10, 0x00, \
	 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71}
#endif