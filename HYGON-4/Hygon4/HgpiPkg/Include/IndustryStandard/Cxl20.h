/** @file
  CXL 2.0 Register definitions

  This file contains the register definitions based on the Compute Express Link
  (CXL) Specification Revision 1.1.

Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _CXL20_H_
#define _CXL20_H_

#include <IndustryStandard/Pci.h>

// CXL 2.0 DVSEC Vendor ID

#define CXL_DVSEC_VENDOR_ID                    0x1E98

/* CXL DVSEC Register Layout and Definition */
//CXL PCIe DVSEC for Flex Bus Device
#define CXL_FLEX_BUS_CAP_OFFSET                 0x0A
#define CXL_FLEX_BUS_CONTROL_OFFSET             0x0C
#define CXL_FLEX_BUS_STATUS_OFFSET              0x0E
#define CXL_FLEX_BUS_CONTROL2_OFFSET            0x10
#define CXL_FLEX_BUS_STATUS2_OFFSET             0x12
#define CXL_FLEX_BUS_LOCK_OFFSET                0x14
#define CXL_FLEX_BUS_CAP2_OFFSET                0x16
#define CXL_FLEX_BUS_RANGE1_SIZE_HIGH_OFFSET    0x18
#define CXL_FLEX_BUS_RANGE1_SIZE_LOW_OFFSET     0x1C
#define CXL_FLEX_BUS_RANGE1_BASE_HIGH_OFFSET    0x20
#define CXL_FLEX_BUS_RANGE1_BASE_LOW_OFFSET     0x24

//CXL 2.0 Extensions DVSEC for Ports
#define CXL_RCRB_SIZE                           0x2000
#define CXL_RCRB_BASE_LOW_OFFSET                0x20
#define CXL_RCRB_BASE_HIGH_OFFSET               0x24
#define CXL_DSP_RCRB_OFFSET                     0
#define CXL_USP_RCRB_OFFSET                     0x1000
#define CXL_COMMAND_OFFSET                      0x04
#define CXL_MEMBAR0_LOW_OFFSET                  0x10
#define CXL_MEMBAR0_HIGH_OFFSET                 0x14
#define CXL_MEM_BASE_LIMIT_OFFSET               0x20

//Flex Bus Port DVSEC
#define FLEX_BUS_PORT_CAP_OFFSET                0x0A
#define FLEX_BUS_PORT_CONTROL_OFFSET            0x0C
#define FLEX_BUS_PORT_STATUS_OFFSET             0x0E

//Register Locator DVSEC
#define REGISTER_BLOCK1_LOW_OFFSET              0x0C
#define REGISTER_BLOCK1_HIGH_OFFSET             0x10

/* CXL Component Register Layout and Definition */
#define CXL_COMPONENT_REG_BLOCK_SIZE             SIZE_64KB

/* CXL.cache and CXL.mem Registers*/
#define CXL_CM_OFFSET                            0x1000
#define CXL_ARB_MUX_OFFSET                       0xE000

typedef enum {
  //CXL 1.1 CM ID
  CXL_CM_CAP_HDR_CAP_ID                 = 1,
  CXL_CM_CAP_CAP_ID_RAS                 = 2,
  CXL_CM_CAP_CAP_ID_SECURITY            = 3,
  CXL_CM_CAP_CAP_ID_LINK                = 4,
  //CXL 2.0 CM ID
  CXL_CM_CAP_CAP_ID_HDM                 = 5,
  CXL_CM_CAP_CAP_ID_EXT_SECURITY        = 6,
  CXL_CM_CAP_CAP_ID_IDE                 = 7,
  CXL_CM_CAP_CAP_ID_SNOOP_FILTER        = 8
} CACHE_MEM_CAP_ID;

typedef enum {
  //CXL 1.1 ID
  CXL_DEV_CAP_ARRAY_CAP_ID              = 0,
  CXL_DEV_CAP_CAP_ID_DEVICE_STATUS      = 1,
  CXL_DEV_CAP_CAP_ID_PRIMARY_MAILBOX    = 2,
  CXL_DEV_CAP_CAP_ID_SECONDARY_MAILBOX  = 3,
  CXL_DEV_CAP_CAP_ID_MEMDEV             = 0x4000,
} CXL_20_DEVICE_CAP_ID;

typedef enum {
  CxlConventionalMemory = 0,
  CxlPersistentMemory = 1
} CXL_MEM_TYPE;

/* Memory Device Status Registers */
#define CXLMDEV_MS_NOT_READY              0
#define CXLMDEV_MS_READY                  1
#define CXLMDEV_MS_ERROR                  2
#define CXLMDEV_MS_DISABLED               3

#define CXLMDEV_RESET_NEEDED_NOT          0
#define CXLMDEV_RESET_NEEDED_COLD         1
#define CXLMDEV_RESET_NEEDED_WARM         2
#define CXLMDEV_RESET_NEEDED_HOT          3
#define CXLMDEV_RESET_NEEDED_CXL          4

/* CXL 2.0 Section 8.2.9 Device Command Opcodes */
typedef enum {
	CXL_MBOX_OP_GET_EVENT_RECORDS    = 0x0100,
	CXL_MBOX_OP_CLEAR_EVENT_RECORDS  = 0x0101,
  CXL_MBOX_OP_GET_EVENT_INT_POLICY = 0x0102,
  CXL_MBOX_OP_SET_EVENT_INT_POLICY = 0x0103,
	CXL_MBOX_OP_GET_FW_INFO		       = 0x0200,
  CXL_MBOX_OP_TRANSFER_FW		       = 0x0201,
	CXL_MBOX_OP_ACTIVATE_FW		       = 0x0202,
  CXL_MBOX_OP_GET_TIMESTAMP	       = 0x0300,
  CXL_MBOX_OP_SET_TIMESTAMP	       = 0x0301,
	CXL_MBOX_OP_GET_SUPPORTED_LOGS	 = 0x0400,
	CXL_MBOX_OP_GET_LOG		           = 0x0401,
	CXL_MBOX_OP_IDENTIFY		         = 0x4000,
	CXL_MBOX_OP_GET_PARTITION_INFO	 = 0x4100,
	CXL_MBOX_OP_SET_PARTITION_INFO	 = 0x4101,
	CXL_MBOX_OP_GET_LSA		           = 0x4102,
	CXL_MBOX_OP_SET_LSA		           = 0x4103,
	CXL_MBOX_OP_GET_HEALTH_INFO	     = 0x4200,
	CXL_MBOX_OP_GET_ALERT_CONFIG	   = 0x4201,
	CXL_MBOX_OP_SET_ALERT_CONFIG	   = 0x4202,
	CXL_MBOX_OP_GET_SHUTDOWN_STATE	 = 0x4203,
	CXL_MBOX_OP_SET_SHUTDOWN_STATE	 = 0x4204,
	CXL_MBOX_OP_GET_POISON		       = 0x4300,
	CXL_MBOX_OP_INJECT_POISON	       = 0x4301,
	CXL_MBOX_OP_CLEAR_POISON	       = 0x4302,
	CXL_MBOX_OP_GET_SCAN_MEDIA_CAPS	 = 0x4303,
	CXL_MBOX_OP_SCAN_MEDIA		       = 0x4304,
	CXL_MBOX_OP_GET_SCAN_MEDIA	     = 0x4305,
  CXL_MBOX_OP_SANITIZE	           = 0x4400,
  CXL_MBOX_OP_SECURE_ERASE	       = 0x4401,
  CXL_MBOX_OP_GET_SECURITY_STATE   = 0x4500,
  CXL_MBOX_OP_SET_PASSPHRASE       = 0x4501,
  CXL_MBOX_OP_DISABLE_PASSPHRASE   = 0x4502,
  CXL_MBOX_OP_UNLOCK               = 0x4503,
  CXL_MBOX_OP_FREEZE_SECURITY_STATE = 0x4504,
  CXL_MBOX_OP_PASSPHRASE_SECURE_ERASE = 0x4505,
  CXL_MBOX_OP_SECURITY_SEND        = 0x4600,
  CXL_MBOX_OP_SECURITY_RECEIVE     = 0x4601,
  CXL_MBOX_OP_GET_SLD_QOS_CONTROL  = 0x4700,
  CXL_MBOX_OP_SET_SLD_QOS_CONTROL  = 0x4701,
  CXL_MBOX_OP_GET_SLD_QOS_STATUS   = 0x4702,
	CXL_MBOX_OP_MAX			             = 0x10000
} CXL_OPCODE;

#define	CXL_CEL_GUID	\
  { 0xda9c0b5, 0xbf41, 0x4b78, 0x8f, 0x79, 0x96, 0xb1, 0x62,  0x3b, 0x3f, 0x17 }

#define	CXL_VENDOR_DEBUG_GUID	\
  { 0xe1819d9, 0x11a9, 0x400c, 0x81, 0x1f, 0xd6, 0x07, 0x19,  0x40, 0x3d, 0x86 }

//
// Ensure proper structure formats
//
#pragma pack(1)

/**
  Macro used to verify the size of a data type at compile time and trigger a
  STATIC_ASSERT() with an error message if the size of the data type does not
  match the expected size.

  @param  TypeName      Type name of data type to verify.
  @param  ExpectedSize  The expected size, in bytes, of the data type specified
                        by TypeName.
**/
#define CXL_20_SIZE_ASSERT(TypeName, ExpectedSize)        \
  STATIC_ASSERT (                                         \
    sizeof (TypeName) == ExpectedSize,                    \
    "Size of " #TypeName                                  \
    " does not meet CXL 2.0 Specification requirements."  \
    )

/**
  Macro used to verify the offset of a field in a data type at compile time and
  trigger a STATIC_ASSERT() with an error message if the offset of the field in
  the data type does not match the expected offset.

  @param  TypeName        Type name of data type to verify.
  @param  FieldName       Field name in the data type specified by TypeName to
                          verify.
  @param  ExpectedOffset  The expected offset, in bytes, of the field specified
                          by TypeName and FieldName.
**/
#define CXL_20_OFFSET_ASSERT(TypeName, FieldName, ExpectedOffset)  \
  STATIC_ASSERT (                                                  \
    OFFSET_OF (TypeName, FieldName) == ExpectedOffset,             \
    "Offset of " #TypeName "." #FieldName                          \
    " does not meet CXL 2.0 Specification requirements."           \
    )

///
/// The PCIe DVSEC for Flex Bus Device (DVSEC ID 0)
///@{
typedef union {
  struct {
    UINT16    CacheCapable             : 1;                          // bit 0
    UINT16    IoCapable                : 1;                          // bit 1
    UINT16    MemCapable               : 1;                          // bit 2
    UINT16    MemHwInitMode            : 1;                          // bit 3
    UINT16    HdmCount                 : 2;                          // bit 4..5
    UINT16    CacheWbAndInvalid        : 1;                          // bit 6
    UINT16    CXLResetCapable          : 1;                          // bit 7
    UINT16    CXLResetTimeout          : 3;                          // bit 8..10
    UINT16    CXLResetMemClrCapable    : 1;                          // bit 11
    UINT16    Reserved                 : 1;                          // bit 12
    UINT16    MultipleLogicalDevice    : 1;                          // bit 13
    UINT16    ViralCapable             : 1;                          // bit 14
    UINT16    PMInitCplReportCapable   : 1;                          // bit 15
  } Bits;
  UINT16    Uint16;
} CXL_2_0_DVSEC_FLEX_BUS_DEVICE_CAPABILITY;

typedef union {
  struct {
    UINT16    DisableCaching            : 1;                          // bit 0
    UINT16    InitiateCacheWbAndInvalid : 1;                          // bit 1
    UINT16    InitiateCXLReset          : 1;                          // bit 2
    UINT16    CXLResetMemClrEnable      : 1;                          // bit 3
    UINT16    Reserved                  : 12;                         // bit 4..15
  } Bits;
  UINT16    Uint16;
} CXL_2_0_DVSEC_FLEX_BUS_DEVICE_CONTROL2;

typedef union {
  struct {
    UINT16    CacheInvalid              : 1;                          // bit 0
    UINT16    CXLResetComplete          : 1;                          // bit 1
    UINT16    CXLResetError             : 1;                          // bit 2
    UINT16    Reserved                  : 12;                         // bit 3..14
    UINT16    PMInitComplete            : 1;                          // bit 15
  } Bits;
  UINT16    Uint16;
} CXL_2_0_DVSEC_FLEX_BUS_DEVICE_STATUS2;

///
/// The PCIe DVSEC for Flex Bus Device
///@{
typedef union {
  struct {
    UINT16    CacheSizeUnit             : 4;                          // bit 0..3
    UINT16    Reserved                  : 4;                          // bit 4..7
    UINT16    CacheSize                 : 8;                          // bit 8..15
  } Bits;
  UINT16    Uint16;
} CXL_2_0_DVSEC_FLEX_BUS_DEVICE_CAPABILITY2;

typedef union {
  struct {
    UINT32    MemoryInfoValid           : 1;                          // bit 0
    UINT32    MemoryActive              : 1;                          // bit 1
    UINT32    MediaType                 : 3;                          // bit 2..4
    UINT32    MemoryClass               : 3;                          // bit 5..7
    UINT32    DesiredInterleave         : 5;                          // bit 8..12
    UINT32    MemoryActiveTimeout       : 3;                          // bit 13..15
    UINT32    Reserved                  : 12;                         // bit 16..27
    UINT32    MemorySizeLow             : 4;                          // bit 28..31
  } Bits;
  UINT32    Uint32;
} CXL_2_0_DVSEC_FLEX_BUS_DEVICE_RANGE1_SIZE_LOW;

typedef union {
  struct {
    UINT32    MemoryInfoValid           : 1;                         // bit 0
    UINT32    MemoryActive              : 1;                         // bit 1
    UINT32    MediaType                 : 3;                         // bit 2..4
    UINT32    MemoryClass               : 3;                         // bit 5..7
    UINT32    DesiredInterleave         : 5;                         // bit 8..12
    UINT32    MemoryActiveTimeout       : 3;                         // bit 13..15
    UINT32    Reserved                  : 12;                        // bit 16..27
    UINT32    MemorySizeLow             : 4;                         // bit 28..31
  } Bits;
  UINT32    Uint32;
} CXL_2_0_DVSEC_FLEX_BUS_DEVICE_RANGE2_SIZE_LOW;

//
// PCIe DVSEC for Flex Bus Device
// Compute Express Link Specification Revision: 2.0 - Chapter 8.1.3, Figure 126
//
typedef struct {
  PCI_EXPRESS_EXTENDED_CAPABILITIES_HEADER           Header;                                      // offset 0
  PCI_EXPRESS_DESIGNATED_VENDOR_SPECIFIC_HEADER_1    DesignatedVendorSpecificHeader1;             // offset 4
  PCI_EXPRESS_DESIGNATED_VENDOR_SPECIFIC_HEADER_2    DesignatedVendorSpecificHeader2;             // offset 8
  CXL_2_0_DVSEC_FLEX_BUS_DEVICE_CAPABILITY           DeviceCapability;                               // offset 10
  CXL_DVSEC_FLEX_BUS_DEVICE_CONTROL                  DeviceControl;                                  // offset 12
  CXL_DVSEC_FLEX_BUS_DEVICE_STATUS                   DeviceStatus;                                   // offset 14
  CXL_2_0_DVSEC_FLEX_BUS_DEVICE_CONTROL2             DeviceControl2;                                 // offset 16
  CXL_2_0_DVSEC_FLEX_BUS_DEVICE_STATUS2              DeviceStatus2;                                  // offset 18
  CXL_DVSEC_FLEX_BUS_DEVICE_LOCK                     DeviceLock;                                     // offset 20
  CXL_2_0_DVSEC_FLEX_BUS_DEVICE_CAPABILITY2          DeviceCapability2;                              // offset 22
  CXL_DVSEC_FLEX_BUS_DEVICE_RANGE1_SIZE_HIGH         DeviceRange1SizeHigh;                        // offset 24
  CXL_2_0_DVSEC_FLEX_BUS_DEVICE_RANGE1_SIZE_LOW      DeviceRange1SizeLow;                         // offset 28
  CXL_DVSEC_FLEX_BUS_DEVICE_RANGE1_BASE_HIGH         DeviceRange1BaseHigh;                        // offset 32
  CXL_DVSEC_FLEX_BUS_DEVICE_RANGE1_BASE_LOW          DeviceRange1BaseLow;                         // offset 36
  CXL_DVSEC_FLEX_BUS_DEVICE_RANGE2_SIZE_HIGH         DeviceRange2SizeHigh;                        // offset 40
  CXL_2_0_DVSEC_FLEX_BUS_DEVICE_RANGE2_SIZE_LOW      DeviceRange2SizeLow;                         // offset 44
  CXL_DVSEC_FLEX_BUS_DEVICE_RANGE2_BASE_HIGH         DeviceRange2BaseHigh;                        // offset 48
  CXL_DVSEC_FLEX_BUS_DEVICE_RANGE2_BASE_LOW          DeviceRange2BaseLow;                         // offset 52
} CXL_2_0_DVSEC_FLEX_BUS_DEVICE;

CXL_20_OFFSET_ASSERT (CXL_2_0_DVSEC_FLEX_BUS_DEVICE, Header, 0x00);
CXL_20_OFFSET_ASSERT (CXL_2_0_DVSEC_FLEX_BUS_DEVICE, DesignatedVendorSpecificHeader1, 0x04);
CXL_20_OFFSET_ASSERT (CXL_2_0_DVSEC_FLEX_BUS_DEVICE, DesignatedVendorSpecificHeader2, 0x08);
CXL_20_OFFSET_ASSERT (CXL_2_0_DVSEC_FLEX_BUS_DEVICE, DeviceCapability, 0x0A);
CXL_20_OFFSET_ASSERT (CXL_2_0_DVSEC_FLEX_BUS_DEVICE, DeviceControl, 0x0C);
CXL_20_OFFSET_ASSERT (CXL_2_0_DVSEC_FLEX_BUS_DEVICE, DeviceStatus, 0x0E);
CXL_20_OFFSET_ASSERT (CXL_2_0_DVSEC_FLEX_BUS_DEVICE, DeviceControl2, 0x10);
CXL_20_OFFSET_ASSERT (CXL_2_0_DVSEC_FLEX_BUS_DEVICE, DeviceStatus2, 0x12);
CXL_20_OFFSET_ASSERT (CXL_2_0_DVSEC_FLEX_BUS_DEVICE, DeviceLock, 0x14);
CXL_20_OFFSET_ASSERT (CXL_2_0_DVSEC_FLEX_BUS_DEVICE, DeviceCapability2, 0x16);
CXL_20_OFFSET_ASSERT (CXL_2_0_DVSEC_FLEX_BUS_DEVICE, DeviceRange1SizeHigh, 0x18);
CXL_20_OFFSET_ASSERT (CXL_2_0_DVSEC_FLEX_BUS_DEVICE, DeviceRange1SizeLow, 0x1C);
CXL_20_OFFSET_ASSERT (CXL_2_0_DVSEC_FLEX_BUS_DEVICE, DeviceRange1BaseHigh, 0x20);
CXL_20_OFFSET_ASSERT (CXL_2_0_DVSEC_FLEX_BUS_DEVICE, DeviceRange1BaseLow, 0x24);
CXL_20_OFFSET_ASSERT (CXL_2_0_DVSEC_FLEX_BUS_DEVICE, DeviceRange2SizeHigh, 0x28);
CXL_20_OFFSET_ASSERT (CXL_2_0_DVSEC_FLEX_BUS_DEVICE, DeviceRange2SizeLow, 0x2C);
CXL_20_OFFSET_ASSERT (CXL_2_0_DVSEC_FLEX_BUS_DEVICE, DeviceRange2BaseHigh, 0x30);
CXL_20_OFFSET_ASSERT (CXL_2_0_DVSEC_FLEX_BUS_DEVICE, DeviceRange2BaseLow, 0x34);
CXL_20_SIZE_ASSERT (CXL_2_0_DVSEC_FLEX_BUS_DEVICE, 0x38);
///@}


///
/// The CXL 2.0 Extensions DVSEC for Ports (DVSEC ID 3)
///@{
typedef union {
  struct {
    UINT16    PortPMInitComplete        : 1;                         // bit 0
    UINT16    Reserved1                 : 13;                        // bit 1..13
    UINT16    ViralStatus               : 1;                         // bit 14
    UINT16    Reserved2                 : 1;                         // bit 15
  } Bits;
  UINT16    Uint16;
} CXL_2_0_DVSEC_CXL_PORT_EXTENSION_STATUS;

typedef union {
  struct {
    UINT16    UnmaskSBR                 : 1;                         // bit 0
    UINT16    UnmaskLinkDisable         : 1;                         // bit 1
    UINT16    AltMemAndIDSpaceEnable    : 1;                         // bit 2
    UINT16    AltBME                    : 1;                         // bit 3
    UINT16    Reserved1                 : 10;                        // bit 4..13 
    UINT16    ViralEnable               : 1;                         // bit 14
    UINT16    Reserved2                 : 1;                         // bit 15
  } Bits;
  UINT16    Uint16;
} CXL_2_0_DVSEC_PORT_CONTROL_EXTENSIONS;

typedef union {
  struct {
    UINT16    Reserved                  : 4;                         // bit 0..3
    UINT16    AltMemBase                : 12;                        // bit 4..15
  } Bits;
  UINT16    Uint16;
} CXL_2_0_DVSEC_ALT_MEM_BASE;

typedef union {
  struct {
    UINT16    Reserved                  : 4;                         // bit 0..3
    UINT16    AltMemLimit               : 12;                        // bit 4..15
  } Bits;
  UINT16    Uint16;
} CXL_2_0_DVSEC_ALT_MEM_LIMIT;

typedef union {
  struct {
    UINT16    Reserved                  : 4;                         // bit 0..3
    UINT16    AltPrefetchMemBase        : 12;                        // bit 4..15
  } Bits;
  UINT16    Uint16;
} CXL_2_0_DVSEC_ALT_PREFETCH_MEM_BASE;

typedef union {
  struct {
    UINT16    Reserved                  : 4;                         // bit 0..3
    UINT16    AltPrefetchMemLimit       : 12;                        // bit 4..15
  } Bits;
  UINT16    Uint16;
} CXL_2_0_DVSEC_ALT_PREFETCH_MEM_LIMIT;

typedef union {
  struct {
    UINT32    RcrbEnable                : 1;                        // bit 0
    UINT32    Reserved                  : 12;                       // bit 1..12
    UINT32    BaseAddressLow            : 19;                       // bit 13..31
  } Bits;
  UINT32    Uint32;
} CXL_2_0_DVSEC_CXL_RCRB_BASE_LOW;

#define CXL_2_0_EXTENSIONS_DVSEC_ID     3
//
// CXL 2.0 Extensions DVSEC for Ports
// Compute Express Link Specification Revision: 2.0 - Chapter 8.1.5, Figure 128
//
typedef struct {
  PCI_EXPRESS_EXTENDED_CAPABILITIES_HEADER           Header;                                      // offset 0
  PCI_EXPRESS_DESIGNATED_VENDOR_SPECIFIC_HEADER_1    DesignatedVendorSpecificHeader1;             // offset 4
  PCI_EXPRESS_DESIGNATED_VENDOR_SPECIFIC_HEADER_2    DesignatedVendorSpecificHeader2;             // offset 8
  CXL_2_0_DVSEC_CXL_PORT_EXTENSION_STATUS            CxlPortExtensionStatus;                      // offset 10
  CXL_2_0_DVSEC_PORT_CONTROL_EXTENSIONS              PortControlExtensions;                       // offset 12
  UINT8                                              AltBusBase;                                  // offset 14
  UINT8                                              AltBusLimit;                                 // offset 15
  CXL_2_0_DVSEC_ALT_MEM_BASE                         AltMemBase;                                  // offset 16
  CXL_2_0_DVSEC_ALT_MEM_LIMIT                        AltMemLimit;                                 // offset 18
  CXL_2_0_DVSEC_ALT_PREFETCH_MEM_BASE                AltPrefetchMemBase;                          // offset 20
  CXL_2_0_DVSEC_ALT_PREFETCH_MEM_LIMIT               AltPrefetchMemLimit;                         // offset 22
  UINT32                                             AltPrefetchMemBaseHigh;                      // offset 24
  UINT32                                             AltPrefetchMemLimitHigh;                     // offset 28
  CXL_2_0_DVSEC_CXL_RCRB_BASE_LOW                    CxlRcrbBaseLow;                              // offset 32
  UINT32                                             CxlRcrbBaseHigh;                             // offset 36
} CXL_2_0_EXTENSIONS_DVSEC_FOR_PORT;

CXL_20_OFFSET_ASSERT (CXL_2_0_EXTENSIONS_DVSEC_FOR_PORT, Header, 0x00);
CXL_20_OFFSET_ASSERT (CXL_2_0_EXTENSIONS_DVSEC_FOR_PORT, DesignatedVendorSpecificHeader1, 0x04);
CXL_20_OFFSET_ASSERT (CXL_2_0_EXTENSIONS_DVSEC_FOR_PORT, DesignatedVendorSpecificHeader2, 0x08);
CXL_20_OFFSET_ASSERT (CXL_2_0_EXTENSIONS_DVSEC_FOR_PORT, CxlPortExtensionStatus, 0x0A);
CXL_20_OFFSET_ASSERT (CXL_2_0_EXTENSIONS_DVSEC_FOR_PORT, PortControlExtensions, 0x0C);
CXL_20_OFFSET_ASSERT (CXL_2_0_EXTENSIONS_DVSEC_FOR_PORT, AltBusBase, 0x0E);
CXL_20_OFFSET_ASSERT (CXL_2_0_EXTENSIONS_DVSEC_FOR_PORT, AltBusLimit, 0x0F);
CXL_20_OFFSET_ASSERT (CXL_2_0_EXTENSIONS_DVSEC_FOR_PORT, AltMemBase, 0x10);
CXL_20_OFFSET_ASSERT (CXL_2_0_EXTENSIONS_DVSEC_FOR_PORT, AltMemLimit, 0x12);
CXL_20_OFFSET_ASSERT (CXL_2_0_EXTENSIONS_DVSEC_FOR_PORT, AltPrefetchMemBase, 0x14);
CXL_20_OFFSET_ASSERT (CXL_2_0_EXTENSIONS_DVSEC_FOR_PORT, AltPrefetchMemLimit, 0x16);
CXL_20_OFFSET_ASSERT (CXL_2_0_EXTENSIONS_DVSEC_FOR_PORT, AltPrefetchMemBaseHigh, 0x18);
CXL_20_OFFSET_ASSERT (CXL_2_0_EXTENSIONS_DVSEC_FOR_PORT, AltPrefetchMemLimitHigh, 0x1C);
CXL_20_OFFSET_ASSERT (CXL_2_0_EXTENSIONS_DVSEC_FOR_PORT, CxlRcrbBaseLow, 0x20);
CXL_20_OFFSET_ASSERT (CXL_2_0_EXTENSIONS_DVSEC_FOR_PORT, CxlRcrbBaseHigh, 0x24);
CXL_20_SIZE_ASSERT (CXL_2_0_EXTENSIONS_DVSEC_FOR_PORT, 0x28);
///@}

///
/// The GPF DVSEC for CXL Port (DVSEC ID 4)
///@{
typedef union {
  struct {
    UINT16    PortGPFPhaseTimeoutBase              : 4;                       // bit 0..3
    UINT16    Reserved1                            : 4;                       // bit 4..7
    UINT16    PortGPFPhaseTimeoutScale             : 4;                       // bit 8..11
    UINT16    Reserved2                            : 4;                       // bit 12..15
  } Bits;
  UINT16    Uint16;
} CXL_2_0_GPF_PHASE_CONTROL;

#define CXL_2_0_GPF_DVSEC_ID     4
//
// GPF DVSEC for CXL Port
// Compute Express Link Specification Revision: 2.0 - Chapter 8.1.6, Figure 129
//
typedef struct {
  PCI_EXPRESS_EXTENDED_CAPABILITIES_HEADER           Header;                                      // offset 0
  PCI_EXPRESS_DESIGNATED_VENDOR_SPECIFIC_HEADER_1    DesignatedVendorSpecificHeader1;             // offset 4
  PCI_EXPRESS_DESIGNATED_VENDOR_SPECIFIC_HEADER_2    DesignatedVendorSpecificHeader2;             // offset 8
  UINT16                                             Reserved;                                    // offset 10
  CXL_2_0_GPF_PHASE_CONTROL                          Phase1;                                   // offset 12
  CXL_2_0_GPF_PHASE_CONTROL                          Phase2;                                   // offset 14
} CXL_2_0_GPF_DVSEC_FOR_PORT;

CXL_20_OFFSET_ASSERT (CXL_2_0_GPF_DVSEC_FOR_PORT, Header, 0x00);
CXL_20_OFFSET_ASSERT (CXL_2_0_GPF_DVSEC_FOR_PORT, DesignatedVendorSpecificHeader1, 0x04);
CXL_20_OFFSET_ASSERT (CXL_2_0_GPF_DVSEC_FOR_PORT, DesignatedVendorSpecificHeader2, 0x08);
CXL_20_OFFSET_ASSERT (CXL_2_0_GPF_DVSEC_FOR_PORT, Phase1, 0x0C);
CXL_20_OFFSET_ASSERT (CXL_2_0_GPF_DVSEC_FOR_PORT, Phase2, 0x0E);
CXL_20_SIZE_ASSERT (CXL_2_0_GPF_DVSEC_FOR_PORT, 0x10);
///@}

///
/// PCIe DVSEC for FLex Bus Port  (DVSEC ID 7)
///@{
typedef union {
  struct {
    UINT16    CacheCapable                   : 1;                     // bit 0
    UINT16    IoCapable                      : 1;                     // bit 1
    UINT16    MemCapable                     : 1;                     // bit 2
    UINT16    Reserved1                      : 2;                     // bit 3..4
    UINT16    CXL2p0Capable                  : 1;                     // bit 5
    UINT16    CXLMultiLogicalDeviceCapable   : 1;                     // bit 6
    UINT16    Reserved2                      : 9;                     // bit 7..15
  } Bits;
  UINT16    Uint16;
} CXL_2_0_DVSEC_FLEX_BUS_PORT_CAPABILITY;

typedef union {
  struct {
    UINT16    CacheEnable                    : 1;                     // bit 0
    UINT16    IoEnable                       : 1;                     // bit 1
    UINT16    MemEnable                      : 1;                     // bit 2
    UINT16    CxlSyncBypassEnable            : 1;                     // bit 3
    UINT16    DriftBufferEnable              : 1;                     // bit 4
    UINT16    CXL2p0Enable                   : 1;                     // bit 5
    UINT16    CXLMultiLogicalDeviceEnable    : 1;                     // bit 6
    UINT16    DisableCXL1p1Training          : 1;                     // bit 7
    UINT16    Retimer1Present                : 1;                     // bit 8
    UINT16    Retimer2Present                : 1;                     // bit 9
    UINT16    Reserved                       : 6;                     // bit 10..15
  } Bits;
  UINT16    Uint16;
} CXL_2_0_DVSEC_FLEX_BUS_PORT_CONTROL;

typedef union {
  struct {
    UINT16    CacheEnable                            : 1;            // bit 0
    UINT16    IoEnable                               : 1;            // bit 1
    UINT16    MemEnable                              : 1;            // bit 2
    UINT16    CxlSyncBypassEnable                    : 1;            // bit 3
    UINT16    DriftBufferEnable                      : 1;            // bit 4
    UINT16    CXL2p0Enable                           : 1;            // bit 5
    UINT16    CXLMultiLogicalDeviceEnable            : 1;            // bit 6
    UINT16    Reserved                               : 1;            // bit 7
    UINT16    CxlCorrectableProtocolIdFramingError   : 1;            // bit 8
    UINT16    CxlUncorrectableProtocolIdFramingError : 1;            // bit 9
    UINT16    CxlUnexpectedProtocolIdDropped         : 1;            // bit 10
    UINT16    CXLRetimersPresentMismatched           : 1;            // bit 11
    UINT16    FlexBusEnableBitsPhase2Mismatch        : 1;            // bit 12
    UINT16    Reserved2                              : 3;            // bit 13..15
  } Bits;
  UINT16    Uint16;
} CXL_2_0_DVSEC_FLEX_BUS_PORT_STATUS;

typedef union {
  struct {
    UINT32    ReceivedFlexBusDataPhase1              : 24;           // bit 0..23
    UINT32    Reserved                               : 8;            // bit 24..31
  } Bits;
  UINT32    Uint32;
} CXL_2_0_DVSEC_FLEX_BUS_PORT_RX_MOD_TS_DATA_PHASE1;
//
// PCIe DVSEC for Flex Bus Port
// Compute Express Link Specification Revision: 2.0 - Chapter 8.2.1.3, Figure 136
//
typedef struct {
  PCI_EXPRESS_EXTENDED_CAPABILITIES_HEADER           Header;                                      // offset 0
  PCI_EXPRESS_DESIGNATED_VENDOR_SPECIFIC_HEADER_1    DesignatedVendorSpecificHeader1;             // offset 4
  PCI_EXPRESS_DESIGNATED_VENDOR_SPECIFIC_HEADER_2    DesignatedVendorSpecificHeader2;             // offset 8
  CXL_2_0_DVSEC_FLEX_BUS_PORT_CAPABILITY             PortCapability;                              // offset 10
  CXL_2_0_DVSEC_FLEX_BUS_PORT_CONTROL                PortControl;                                 // offset 12
  CXL_2_0_DVSEC_FLEX_BUS_PORT_STATUS                 PortStatus;                                  // offset 14
  CXL_2_0_DVSEC_FLEX_BUS_PORT_RX_MOD_TS_DATA_PHASE1  PortRxDataPhase1;                            // offset 16
} CXL_2_0_DVSEC_FLEX_BUS_PORT;

CXL_20_OFFSET_ASSERT (CXL_2_0_DVSEC_FLEX_BUS_PORT, Header, 0x00);
CXL_20_OFFSET_ASSERT (CXL_2_0_DVSEC_FLEX_BUS_PORT, DesignatedVendorSpecificHeader1, 0x04);
CXL_20_OFFSET_ASSERT (CXL_2_0_DVSEC_FLEX_BUS_PORT, DesignatedVendorSpecificHeader2, 0x08);
CXL_20_OFFSET_ASSERT (CXL_2_0_DVSEC_FLEX_BUS_PORT, PortCapability, 0x0A);
CXL_20_OFFSET_ASSERT (CXL_2_0_DVSEC_FLEX_BUS_PORT, PortControl, 0x0C);
CXL_20_OFFSET_ASSERT (CXL_2_0_DVSEC_FLEX_BUS_PORT, PortStatus, 0x0E);
CXL_20_OFFSET_ASSERT (CXL_2_0_DVSEC_FLEX_BUS_PORT, PortRxDataPhase1, 0x10);
CXL_20_SIZE_ASSERT (CXL_2_0_DVSEC_FLEX_BUS_PORT, 0x14);

///
/// The Register Locator DVSEC (DVSEC ID 8)
///@{
typedef union {
  struct {
    UINT32    RegisterBIR                           : 3;                       // bit 0..2
    UINT32    Reserved1                             : 5;                       // bit 3..7
    UINT32    RegisterBlockId                       : 8;                       // bit 8..15
    UINT32    RegisterBlockOffsetLow                : 16;                      // bit 16..31
  } Bits;
  UINT32   Uint32;
} CXL_2_0_REGISTER_BLOCK_OFFSET_LOW;

#define CXL_2_0_REGISTER_LOCATOR_DVSEC_ID     8
#define CXL20_REG_BLOCK_COUNT                 3 

typedef enum {
    EMPTY_BLOCK   = 0,
    COMPONENT_REG_BLOCK  = 1,
    VIRTUAL_ACL_BLOCK  = 2,
    MEMORY_DEVICE_BLOCK = 3
} REGISTER_BLOCK_ID;

//
// GPF DVSEC for CXL Port
// Compute Express Link Specification Revision: 2.0 - Chapter 8.1.9, Figure 131
//
typedef struct {
  PCI_EXPRESS_EXTENDED_CAPABILITIES_HEADER           Header;                                      // offset 0
  PCI_EXPRESS_DESIGNATED_VENDOR_SPECIFIC_HEADER_1    DesignatedVendorSpecificHeader1;             // offset 4
  PCI_EXPRESS_DESIGNATED_VENDOR_SPECIFIC_HEADER_2    DesignatedVendorSpecificHeader2;             // offset 8
  UINT16                                             Reserved;                                    // offset 10
  CXL_2_0_REGISTER_BLOCK_OFFSET_LOW                  Block1RegOffsetLow;                          // offset 12
  UINT32                                             Block1RegOffsetHigh;                         // offset 16
  CXL_2_0_REGISTER_BLOCK_OFFSET_LOW                  Block2RegOffsetLow;                          // offset 20
  UINT32                                             Block2RegOffsetHigh;                         // offset 24
  CXL_2_0_REGISTER_BLOCK_OFFSET_LOW                  Block3RegOffsetLow;                          // offset 28
  UINT32                                             Block3RegOffsetHigh;                         // offset 32
} CXL_2_0_REGISTER_LOCATOR_DVSEC;

CXL_20_OFFSET_ASSERT (CXL_2_0_REGISTER_LOCATOR_DVSEC, Header, 0x00);
CXL_20_OFFSET_ASSERT (CXL_2_0_REGISTER_LOCATOR_DVSEC, DesignatedVendorSpecificHeader1, 0x04);
CXL_20_OFFSET_ASSERT (CXL_2_0_REGISTER_LOCATOR_DVSEC, DesignatedVendorSpecificHeader2, 0x08);
CXL_20_OFFSET_ASSERT (CXL_2_0_REGISTER_LOCATOR_DVSEC, Block1RegOffsetLow, 0x0C);
CXL_20_OFFSET_ASSERT (CXL_2_0_REGISTER_LOCATOR_DVSEC, Block2RegOffsetLow, 0x14);
CXL_20_OFFSET_ASSERT (CXL_2_0_REGISTER_LOCATOR_DVSEC, Block3RegOffsetLow, 0x1C);
CXL_20_SIZE_ASSERT (CXL_2_0_REGISTER_LOCATOR_DVSEC, 0x24);
///@}

///
/// CXL 2.0 Upstream and Downstream Port Subsystem Component registers
///

/// The CXL.Cache and CXL.Memory Architectural register definitions
/// Based on chapter 8.2.5 of Compute Express Link Specification Revision: 2.0
///@{
typedef union {
  struct {
    UINT32    CxlCapabilityId          : 16;                          // bit 0..15
    UINT32    CxlCapabilityVersion     :  4;                          // bit 16..19
    UINT32    CxlCapabilityPointer     : 12;                          // bit 20..31
  } Bits;
  UINT32    Uint32;
} CXL_COMMON_CAPABILITY_HEADER;

typedef union {
  struct {
    UINT32    CacheDataParity       :  1;                             // bit 0
    UINT32    CacheAddressParity    :  1;                             // bit 1
    UINT32    CacheByteEnableParity :  1;                             // bit 2
    UINT32    CacheDataEcc          :  1;                             // bit 3
    UINT32    MemDataParity         :  1;                             // bit 4
    UINT32    MemAddressParity      :  1;                             // bit 5
    UINT32    MemByteEnableParity   :  1;                             // bit 6
    UINT32    MemDataEcc            :  1;                             // bit 7
    UINT32    ReInitThreshold       :  1;                             // bit 8
    UINT32    RsvdEncodingViolation :  1;                             // bit 9
    UINT32    PoisonReceived        :  1;                             // bit 10
    UINT32    ReceiverOverflow      :  1;                             // bit 11
    UINT32    Reserved1             :  2;                             // bit 12..13
    UINT32    InternalError         :  1;                             // bit 14
    UINT32    CxlIdeTxError         :  1;                             // bit 15
    UINT32    CxlIdeRxError         :  1;                             // bit 16
    UINT32    Reserved2             :  15;                            // bit 17..31
  } Bits;
  UINT32    Uint32;
} CXL_2_0_UNCORRECTABLE_ERROR_STATUS;

typedef union {
  struct {
    UINT32    CacheDataParityMask       :  1;                         // bit 0..0
    UINT32    CacheAddressParityMask    :  1;                         // bit 1..1
    UINT32    CacheByteEnableParityMask :  1;                         // bit 2..2
    UINT32    CacheDataEccMask          :  1;                         // bit 3..3
    UINT32    MemDataParityMask         :  1;                         // bit 4..4
    UINT32    MemAddressParityMask      :  1;                         // bit 5..5
    UINT32    MemByteEnableParityMask   :  1;                         // bit 6..6
    UINT32    MemDataEccMask            :  1;                         // bit 7..7
    UINT32    ReInitThresholdMask       :  1;                         // bit 8..8
    UINT32    RsvdEncodingViolationMask :  1;                         // bit 9..9
    UINT32    PoisonReceivedMask        :  1;                         // bit 10..10
    UINT32    ReceiverOverflowMask      :  1;                         // bit 11..11
    UINT32    Reserved1                 :  2;                         // bit 12..13
    UINT32    InternalErrorMask         :  1;                         // bit 14
    UINT32    CxlIdeTxErrorMask         :  1;                         // bit 15
    UINT32    CxlIdeRxErrorMask         :  1;                         // bit 16
    UINT32    Reserved2                 :  15;                        // bit 17..31
  } Bits;
  UINT32    Uint32;
} CXL_2_0_UNCORRECTABLE_ERROR_MASK;

typedef union {
  struct {
    UINT32    CacheDataParitySeverity       :  1;                     // bit 0..0
    UINT32    CacheAddressParitySeverity    :  1;                     // bit 1..1
    UINT32    CacheByteEnableParitySeverity :  1;                     // bit 2..2
    UINT32    CacheDataEccSeverity          :  1;                     // bit 3..3
    UINT32    MemDataParitySeverity         :  1;                     // bit 4..4
    UINT32    MemAddressParitySeverity      :  1;                     // bit 5..5
    UINT32    MemByteEnableParitySeverity   :  1;                     // bit 6..6
    UINT32    MemDataEccSeverity            :  1;                     // bit 7..7
    UINT32    ReInitThresholdSeverity       :  1;                     // bit 8..8
    UINT32    RsvdEncodingViolationSeverity :  1;                     // bit 9..9
    UINT32    PoisonReceivedSeverity        :  1;                     // bit 10..10
    UINT32    ReceiverOverflowSeverity      :  1;                     // bit 11..11
    UINT32    Reserved1                     :  2;                     // bit 12..13
    UINT32    InternalErrorSeverity         :  1;                     // bit 14
    UINT32    CxlIdeTxErrorSeverity         :  1;                     // bit 15
    UINT32    CxlIdeRxErrorSeverity         :  1;                     // bit 16
    UINT32    Reserved2                     :  15;                    // bit 17..31
  } Bits;
  UINT32    Uint32;
} CXL_2_0_UNCORRECTABLE_ERROR_SEVERITY;

typedef union {
  struct {
    UINT32    FirstErrorPointer                 :  6;                 // bit 0..5
    UINT32    Reserved1                         :  3;                 // bit 6..8
    UINT32    MultipleHeaderRecordingCapability :  1;                 // bit 9..9
    UINT32    Reserved2                         :  3;                 // bit 10..12
    UINT32    PoisonEnabled                     :  1;                 // bit 13..13
    UINT32    Reserved3                         : 18;                 // bit 14..31
  } Bits;
  UINT32    Uint32;
} CXL_2_0_ERROR_CAPABILITIES_AND_CONTROL;

typedef struct {
  CXL_2_0_UNCORRECTABLE_ERROR_STATUS      UncorrectableErrorStatus;
  CXL_2_0_UNCORRECTABLE_ERROR_MASK        UncorrectableErrorMask;
  CXL_2_0_UNCORRECTABLE_ERROR_SEVERITY    UncorrectableErrorSeverity;
  CXL_CORRECTABLE_ERROR_STATUS            CorrectableErrorStatus;
  CXL_CORRECTABLE_ERROR_MASK              CorrectableErrorMask;
  CXL_2_0_ERROR_CAPABILITIES_AND_CONTROL  ErrorCapabilitiesAndControl;
  UINT32                                  HeaderLog[16];
} CXL_2_0_RAS_CAPABILITY_STRUCTURE;

CXL_20_OFFSET_ASSERT (CXL_2_0_RAS_CAPABILITY_STRUCTURE, UncorrectableErrorStatus, 0x00);
CXL_20_OFFSET_ASSERT (CXL_2_0_RAS_CAPABILITY_STRUCTURE, UncorrectableErrorMask, 0x04);
CXL_20_OFFSET_ASSERT (CXL_2_0_RAS_CAPABILITY_STRUCTURE, UncorrectableErrorSeverity, 0x08);
CXL_20_OFFSET_ASSERT (CXL_2_0_RAS_CAPABILITY_STRUCTURE, CorrectableErrorStatus, 0x0C);
CXL_20_OFFSET_ASSERT (CXL_2_0_RAS_CAPABILITY_STRUCTURE, CorrectableErrorMask, 0x10);
CXL_20_OFFSET_ASSERT (CXL_2_0_RAS_CAPABILITY_STRUCTURE, ErrorCapabilitiesAndControl, 0x14);
CXL_20_OFFSET_ASSERT (CXL_2_0_RAS_CAPABILITY_STRUCTURE, HeaderLog, 0x18);
CXL_20_SIZE_ASSERT (CXL_2_0_RAS_CAPABILITY_STRUCTURE, 0x58);

typedef union {
  struct {
    UINT64    CxlLinkVersionSupported :  4;                           // bit 0..3
    UINT64    CxlLinkVersionReceived  :  4;                           // bit 4..7
    UINT64    LlrWrapValueSupported   :  8;                           // bit 8..15
    UINT64    LlrWrapValueReceived    :  8;                           // bit 16..23
    UINT64    NumRetryReceived        :  5;                           // bit 24..28
    UINT64    NumPhyReinitReceived    :  5;                           // bit 29..33
    UINT64    WrPtrReceived           :  8;                           // bit 34..41
    UINT64    EchoEseqReceived        :  8;                           // bit 42..49
    UINT64    NumFreeBufReceived      :  8;                           // bit 50..57
    UINT64    NoLLResetSupport        :  1;                           // bit 58
    UINT64    Reserved                :  5;                           // bit 59..63
  } Bits;
  UINT64    Uint64;
} CXL_2_0_LINK_LAYER_CAPABILITY;

typedef struct {
  CXL_2_0_LINK_LAYER_CAPABILITY             LinkLayerCapability;
  CXL_LINK_LAYER_CONTROL_AND_STATUS         LinkLayerControlStatus;
  CXL_LINK_LAYER_RX_CREDIT_CONTROL          LinkLayerRxCreditControl;
  CXL_LINK_LAYER_RX_CREDIT_RETURN_STATUS    LinkLayerRxCreditReturnStatus;
  CXL_LINK_LAYER_TX_CREDIT_STATUS           LinkLayerTxCreditStatus;
  CXL_LINK_LAYER_ACK_TIMER_CONTROL          LinkLayerAckTimerControl;
  CXL_LINK_LAYER_DEFEATURE                  LinkLayerDefeature;
} CXL_2_0_LINK_CAPABILITY_STRUCTURE;

CXL_20_OFFSET_ASSERT (CXL_2_0_LINK_CAPABILITY_STRUCTURE, LinkLayerCapability, 0x00);
CXL_20_OFFSET_ASSERT (CXL_2_0_LINK_CAPABILITY_STRUCTURE, LinkLayerControlStatus, 0x08);
CXL_20_OFFSET_ASSERT (CXL_2_0_LINK_CAPABILITY_STRUCTURE, LinkLayerRxCreditControl, 0x10);
CXL_20_OFFSET_ASSERT (CXL_2_0_LINK_CAPABILITY_STRUCTURE, LinkLayerRxCreditReturnStatus, 0x18);
CXL_20_OFFSET_ASSERT (CXL_2_0_LINK_CAPABILITY_STRUCTURE, LinkLayerTxCreditStatus, 0x20);
CXL_20_OFFSET_ASSERT (CXL_2_0_LINK_CAPABILITY_STRUCTURE, LinkLayerAckTimerControl, 0x28);
CXL_20_OFFSET_ASSERT (CXL_2_0_LINK_CAPABILITY_STRUCTURE, LinkLayerDefeature, 0x30);
CXL_20_SIZE_ASSERT (CXL_2_0_LINK_CAPABILITY_STRUCTURE, 0x38);

typedef union {
  struct {
    UINT32    DecoderCount                    :  4;                           // bit 0..3
    UINT32    TargetCount                     :  4;                           // bit 4..7
    UINT32    AddrBit11InterleaveSupport      :  1;                           // bit 8
    UINT32    AddrBit14InterleaveSupport      :  1;                           // bit 9
    UINT32    PoisonOnDecodeErrorCapability   :  1;                           // bit 10
    UINT32    Reserved                        : 21;                           // bit 11~31
  } Bits;
  UINT32    Uint32;
} CXL_HDM_DECODER_CAPABILITY;

typedef union {
  struct {
    UINT32    PoisonOnDecodeErrorEnable       :  1;                           // bit 0
    UINT32    HdmDecoderEnable                :  1;                           // bit 1
    UINT32    Reserved                        : 30;                           // bit 2~31
  } Bits;
  UINT32    Uint32;
} CXL_HDM_DECODER_GLOBAL_CONTROL;

typedef union {
  struct {
    UINT32    Reserved                        : 28;                           // bit 0~27
    UINT32    MemoryBaseLow                   : 4;                            // bit 28~31
  } Bits;
  UINT32    Uint32;
} CXL_HDM_DECODER_BASE_LOW;

typedef union {
  struct {
    UINT32    Reserved                        : 28;                           // bit 0~27
    UINT32    MemorySizeLow                   : 4;                            // bit 28~31
  } Bits;
  UINT32    Uint32;
} CXL_HDM_DECODER_SIZE_LOW;

typedef union {
  struct {
    UINT32    InterleaveGranularity           :  4;                           // bit 0..3
    UINT32    InterleaveWays                  :  4;                           // bit 4..7
    UINT32    LockOnCommit                    :  1;                           // bit 8
    UINT32    Commit                          :  1;                           // bit 9
    UINT32    Committed                       :  1;                           // bit 10
    UINT32    ErrorNotCommitted               :  1;                           // bit 11
    UINT32    TargetDeviceType                :  1;                           // bit 12
    UINT32    Reserved                        : 19;                           // bit 13..31
  } Bits;
  UINT32    Uint32;
} CXL_HDM_DECODER_CONTROL;

/* TARGER LIST register is only applicable to CXL 2.0 DSP */
typedef union {
  struct {
    UINT32    TargetPortIdForIW0              : 8;                            // bit 0~7
    UINT32    TargetPortIdForIW1              : 8;                            // bit 8~15
    UINT32    TargetPortIdForIW2              : 8;                            // bit 16~23
    UINT32    TargetPortIdForIW3              : 8;                            // bit 24~31
  } Bits;
  UINT32    Uint32;
} CXL_HDM_DECODER_TARGER_LIST_LOW;

typedef union {
  struct {
    UINT32    TargetPortIdForIW4              : 8;                            // bit 0~7
    UINT32    TargetPortIdForIW5              : 8;                            // bit 8~15
    UINT32    TargetPortIdForIW6              : 8;                            // bit 16~23
    UINT32    TargetPortIdForIW7              : 8;                            // bit 24~31
  } Bits;
  UINT32    Uint32;
} CXL_HDM_DECODER_TARGER_LIST_HIGH;

/* DPA Skip register is only applicable to CXL 2.0 devices */
typedef union {
  struct {
    UINT32    Reserved                        : 28;                           // bit 0~27
    UINT32    DpaSkipLow                      : 4;                            // bit 28~31
  } Bits;
  UINT32    Uint32;
} CXL_HDM_DECODER_DPA_SKIP_LOW;

/* CXL HDM Decoder Capability Structure */
typedef struct {
  CXL_HDM_DECODER_CAPABILITY                HdmDecoderCapability;
  CXL_HDM_DECODER_GLOBAL_CONTROL            HdmDecoderGlobalControl;
  UINT64                                    Reserved;
  CXL_HDM_DECODER_BASE_LOW                  HdmDecoder0BaseLow;
  UINT32                                    HdmDecoder0BaseHigh;
  CXL_HDM_DECODER_SIZE_LOW                  HdmDecoder0SizeLow;
  UINT32                                    HdmDecoder0SizeHigh;
  CXL_HDM_DECODER_CONTROL                   HdmDecoder0Control;
  CXL_HDM_DECODER_TARGER_LIST_LOW           HdmDecoder0TargetListLow;
  CXL_HDM_DECODER_TARGER_LIST_HIGH          HdmDecoder0TargetListHigh;
  CXL_HDM_DECODER_BASE_LOW                  HdmDecoder1BaseLow;
  UINT32                                    HdmDecoder1BaseHigh;
  CXL_HDM_DECODER_SIZE_LOW                  HdmDecoder1SizeLow;
  UINT32                                    HdmDecoder1SizeHigh;
  CXL_HDM_DECODER_CONTROL                   HdmDecoder1Control;
  CXL_HDM_DECODER_TARGER_LIST_LOW           HdmDecoder1TargetListLow;
  CXL_HDM_DECODER_TARGER_LIST_HIGH          HdmDecoder1TargetListHigh;
} CXL_2_0_HDM_DECODER_CAPABILITY_DSP_STRUCTURE;

typedef struct {
  CXL_HDM_DECODER_CAPABILITY                HdmDecoderCapability;
  CXL_HDM_DECODER_GLOBAL_CONTROL            HdmDecoderGlobalControl;
  UINT64                                    Reserved;
  CXL_HDM_DECODER_BASE_LOW                  HdmDecoder0BaseLow;
  UINT32                                    HdmDecoder0BaseHigh;
  CXL_HDM_DECODER_SIZE_LOW                  HdmDecoder0SizeLow;
  UINT32                                    HdmDecoder0SizeHigh;
  CXL_HDM_DECODER_CONTROL                   HdmDecoder0Control;
  CXL_HDM_DECODER_DPA_SKIP_LOW              HdmDecoder0DpaSkipLow;
  UINT32                                    HdmDecoder0DpaSkipHigh;
  CXL_HDM_DECODER_BASE_LOW                  HdmDecoder1BaseLow;
  UINT32                                    HdmDecoder1BaseHigh;
  CXL_HDM_DECODER_SIZE_LOW                  HdmDecoder1SizeLow;
  UINT32                                    HdmDecoder1SizeHigh;
  CXL_HDM_DECODER_CONTROL                   HdmDecoder1Control;
  CXL_HDM_DECODER_DPA_SKIP_LOW              HdmDecoder1DpaSkipLow;
  UINT32                                    HdmDecoder1DpaSkipHigh;
} CXL_2_0_HDM_DECODER_CAPABILITY_DEVICE_STRUCTURE;

CXL_20_OFFSET_ASSERT (CXL_2_0_HDM_DECODER_CAPABILITY_DSP_STRUCTURE, HdmDecoderCapability, 0x00);
CXL_20_OFFSET_ASSERT (CXL_2_0_HDM_DECODER_CAPABILITY_DSP_STRUCTURE, HdmDecoderGlobalControl, 0x04);
CXL_20_OFFSET_ASSERT (CXL_2_0_HDM_DECODER_CAPABILITY_DSP_STRUCTURE, HdmDecoder0BaseLow, 0x10);
CXL_20_OFFSET_ASSERT (CXL_2_0_HDM_DECODER_CAPABILITY_DSP_STRUCTURE, HdmDecoder0BaseHigh, 0x14);
CXL_20_OFFSET_ASSERT (CXL_2_0_HDM_DECODER_CAPABILITY_DSP_STRUCTURE, HdmDecoder0SizeLow, 0x18);
CXL_20_OFFSET_ASSERT (CXL_2_0_HDM_DECODER_CAPABILITY_DSP_STRUCTURE, HdmDecoder0SizeHigh, 0x1C);
CXL_20_OFFSET_ASSERT (CXL_2_0_HDM_DECODER_CAPABILITY_DSP_STRUCTURE, HdmDecoder0Control, 0x20);
CXL_20_OFFSET_ASSERT (CXL_2_0_HDM_DECODER_CAPABILITY_DSP_STRUCTURE, HdmDecoder0TargetListLow, 0x24);
CXL_20_OFFSET_ASSERT (CXL_2_0_HDM_DECODER_CAPABILITY_DSP_STRUCTURE, HdmDecoder0TargetListHigh, 0x28);

///@}

///
/// CXL 2.0 Device registers
///

/// The CXL Device Register Interface definitions
/// Based on chapter 8.2.8 of Compute Express Link Specification Revision: 2.0
///@{

/* CXL 2.0 8.2.8.1 Device Capabilities Array Register */
typedef union {
  struct {
    UINT64    CapabilityID                    :  16;                          // bit 0..15
    UINT64    Version                         :  8;                           // bit 16..23
    UINT64    Reserved1                       :  8;                           // bit 24~31
    UINT64    CapabilitiesCount               :  16;                          // bit 32..47
    UINT64    Reserved2                       :  16;                          // bit 48..63
  } Bits;
  UINT64    Uint64;
} CXL_2_0_DEVICE_CAPABILITY_ARRAY;

typedef struct {
    UINT32    CxlCapabilityId                 : 16;                          // bit 0..15
    UINT32    CxlCapabilityVersion            :  8;                          // bit 16..23
    UINT32    Reserved1                       :  8;                          // bit 24..31
    UINT32    Offset;                                                        // bit 32..63
    UINT32    Length;                                                        // bit 64..95
    UINT32    Reserved;                                                      // bit 96..127
} CXL_2_0_DEVICE_CAPABILITY_HEADER_STRUCTURE;

/* Mailbox Registers */
typedef union {
  struct {
    UINT32    PayloadSize                     :  5;                          // bit 0..4
    UINT32    MbDoorbellIntCapable            :  1;                          // bit 5
    UINT32    BgCommandCplIntCapable          :  1;                          // bit 6
    UINT32    IntMessageNumber                :  4;                          // bit 7..10
    UINT32    Reserved                        :  21;                         // bit 11..31
  } Bits;
  UINT32    Uint32;
} CXL_MAILBOX_CAPABILITY;

typedef union {
  struct {
    UINT32    Doorbell                        :  1;                          // bit 0
    UINT32    MbDoorbellIntEnable             :  1;                          // bit 1
    UINT32    BgCommandCplIntEnable           :  1;                          // bit 2
    UINT32    Reserved                        :  29;                         // bit 3..31
  } Bits;
  UINT32    Uint32;
} CXL_MAILBOX_CONTROL;

typedef union {
  struct {
    UINT64    CommandOpcode                   :  16;                         // bit 0..15
    UINT64    PayloadLength                   :  21;                         // bit 16..36
    UINT64    Reserved                        :  27;                         // bit 37..63
  } Bits;
  UINT64    Uint64;
} CXL_MAILBOX_COMMAND;

typedef union {
  struct {
    UINT64    BackgroundOperation             :  1;                          // bit 0
    UINT64    Reserved                        :  31;                         // bit 1..31
    UINT64    ReturnCode                      :  16;                         // bit 32..47
    UINT64    VendorSpecificExtendedStatus    :  16;                         // bit 48..63
  } Bits;
  UINT64    Uint64;
} CXL_MAILBOX_STATUS;

typedef union {
  struct {
    UINT64    CommandOpcode                   :  16;                         // bit 0..15
    UINT64    PercentageComplete              :  7;                          // bit 16..22
    UINT64    Reserved                        :  9;                          // bit 23..31
    UINT64    ReturnCode                      :  16;                         // bit 32..47
    UINT64    VendorSpecificExtendedStatus    :  16;                         // bit 48..63
  } Bits;
  UINT64    Uint64;
} CXL_MAILBOX_BG_STATUS;

typedef struct {
    CXL_MAILBOX_CAPABILITY                CxlMailboxCapability;
    CXL_MAILBOX_CONTROL                   CxlMailboxControl;
    CXL_MAILBOX_COMMAND                   CxlMailboxCommand;
    CXL_MAILBOX_STATUS                    CxlMailboxStatus;
    CXL_MAILBOX_BG_STATUS                 CxlMailboxBgStatus;
} CXL_2_0_DEVICE_MAILBOX_REGISTERS_STRUCTURE;

CXL_20_OFFSET_ASSERT (CXL_2_0_DEVICE_MAILBOX_REGISTERS_STRUCTURE, CxlMailboxCapability, 0x00);
CXL_20_OFFSET_ASSERT (CXL_2_0_DEVICE_MAILBOX_REGISTERS_STRUCTURE, CxlMailboxControl, 0x04);
CXL_20_OFFSET_ASSERT (CXL_2_0_DEVICE_MAILBOX_REGISTERS_STRUCTURE, CxlMailboxCommand, 0x08);
CXL_20_OFFSET_ASSERT (CXL_2_0_DEVICE_MAILBOX_REGISTERS_STRUCTURE, CxlMailboxStatus, 0x10);
CXL_20_OFFSET_ASSERT (CXL_2_0_DEVICE_MAILBOX_REGISTERS_STRUCTURE, CxlMailboxBgStatus, 0x18);
///@}

/// The CXL Memory Device Registers definitions
/// Based on chapter 8.2.8.5 of Compute Express Link Specification Revision: 2.0
///@{
typedef union {
  struct {
    UINT64    DeviceFatal                     :  1;                         // bit 0
    UINT64    FWHalt                          :  1;                         // bit 1
    UINT64    MediaStatus                     :  2;                         // bit 2..3
    UINT64    MailboxInterfacesReady          :  1;                         // bit 4
    UINT64    ResetNeeded                     :  3;                         // bit 5..7
    UINT64    Reserved                        : 56;                         // bit 8..63
  } Bits;
  UINT64    Uint64;
} CXL_MEMORY_DEVICE_STATUS;

///@}

#pragma pack()

#endif
