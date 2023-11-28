/** @file

Copyright (c) 2006 - 2016, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  EfiServerManagement.h

Abstract:
  Source file for the IPMI driver.

Revision History:

**/

#ifndef _EFI_SERVER_MANAGEMENT_H
#define _EFI_SERVER_MANAGEMENT_H

#include "IpmiDefinitions.h"

//
// Byosoft Net function definition for App command
//
#define SM_BYOSOFT_NETFN_APP      0x3e

// OEM send hardware information to BMC
#define SM_BYOSOFT_NETFN_SUB_FUN  0x7a

//
// Defines
//
#define PRIM_IPMB_CHANNEL       0x0
#define BMC_LUN                 0x0
#define PRESENT_INTERFACE       0xE
#define SYSTEM_INTERFACE        0xF
#define EFI_COMPLETE_SEL_RECORD 0xFF
#define IPMI_UNSPECIFIED_ERROR  0xFF

//
// Net Function Defines.
//
#define EFI_SM_CHASSIS      0x0
#define EFI_SM_BRIDGE       0x2
#define EFI_SM_SENSOR_EVENT 0x4
#define EFI_SM_APPLICATION  0x6
#define EFI_SM_FIRMWARE     0x8
#define EFI_SM_STORAGE      0xA
#define EFI_SM_TRANSPORT    0xC
#define EFI_SM_GROUP_EXT    0x2C
#define EFI_SM_OEM_GROUP    0x2E
#define EFI_SM_INTEL_OEM    0x30
#define EFI_SM_SOL_OEM      0x34

//
// IPMI Command Definations.
//
#define EFI_IPMI_GET_DEVICE_ID          1
#define EFI_IPMI_COLD_RESET             2
#define EFI_IPMI_WARM_RESET             3
#define EFI_IPMI_GET_SELF_TEST_RESULTS  4
#define EFI_IPMI_MFG_MODE_ON            5
#define EFI_IPMI_SET_ACPI_POWER_STATE   6
#define EFI_IPMI_GET_ACPI_POWER_STATE   7
#define EFI_IPMI_GET_DEVICE_GUID        8

#define EFI_IPMI_GET_MESSAGE_FLAGS      0x31
#define EFI_IPMI_GET_MESSAGE            0x33
#define EFI_IPMI_SEND_MESSAGE           0x34

#define RESERVE_SEL_ENTRY               0x42
#define ADD_SEL_ENTRY                   0x44
#define GET_SEL_ENTRY                   0x43
#define DELETE_SEL_ENTRY                0x46
#define CLEAR_SEL_ENTRY                 0x47
#define SET_BMC_GLOBALS                 0x2E
#define GET_BMC_GLOBALS                 0x2F
#define SET_SEL_TIME                    0x49

#define GET_SELF_TEST_RESULTS           0x4

#define NMI_ENABLE_DISABLE              0xF7

//
// Controller Attributes
//
#define EFI_IPMI_SENSOR_DEVICE_SUPPORT    0x1
#define EFI_IPMB_SDR_REPOSITORY_SUPPORT   0x2
#define EFI_IPMI_SEL_DEVICE_SUPPORT       0x4
#define EFI_IPMI_FRU_INVENTORY_SUPPORT    0x8
#define EFI_IPMB_EVENT_RECEIVER_SUPPORT   0x10
#define EFI_IPMB_EVENT_GENERATOR_SUPPORT  0x20
#define EFI_ICMB_BRIDGE_SUPPORT           0x40
#define EFI_ICMB_CHASSIS_DEVICE_SUPPORT   0x80
#define EFI_SM_TCP_SUPPORT                0x100
#define EFI_SM_UDP_SUPPORT                0x200
#define EFI_SM_IPV4_SUPPORT               0x400
#define EFI_SM_IPV6_SUPPORT               0x800
#define EFI_SM_RS232_SUPPORT              0x1000

//
// Sensor Type Definations
//
typedef enum {
  EfiSensorReserved,
  EfiSensorTemperature,
  EfiSensorVoltage,
  EfiSensorCurrent,
  EfiSensorFan,
  EfiSensorPhysicalSecurity,
  EfiSensorPlatformSecurityViolationAttempt,
  EfiSensorProcessor,
  EfiSensorPowerSupply,
  EfiSensorPowerUnit,
  EfiSensorCoolingDevice,
  EfiSensorOtherUnits,
  EfiSensorMemory,
  EfiSensorDriveSlot,
  EfiSensorPOSTMemoryResize,
  EfiSensorSystemFirmwareProgress,
  EfiSensorEventLoggingDisabled,
  EfiSensorWatchdog1,
  EfiSensorSystemEvent,
  EfiSensorCriticalInterrupt,
  EfiSensorButton,
  EfiSensorModuleBoard,
  EfiSensorMicrocontrollerCoprocessor,
  EfiSensorAddinCard,
  EfiSensorChassis,
  EfiSensorChipSet,
  EfiSensorOtherFRU,
  EfiSensorCableInterconnect,
  EfiSensorTerminator,
  EfiSensorSystemBootInitiated,
  EfiSensorBootError,
  EfiSensorOSBoot,
  EfiSensorOSCriticalStop,
  EfiSensorSlotConnector,
  EfiSensorSystemACPIPowerState,
  EfiSensorWatchdog2,
  EfiSensorPlatformAlert,
  EfiSensorEntityPresence,
  EfiSensorMonitorASIC,
  EfiSensorLAN,
  EfiSensorManagementSubsystemHealth
} EFI_SM_SENSOR_TYPE;

//
// Sensor Event Type Code
//
#define EFI_SENSOR_THRESHOLD_EVENT_TYPE 1
#define EFI_SENSOR_SPECIFIC_EVENT_TYPE  0x6F

//
// THRESHOLD SENSOR TYPE BIT MASK
//
#define LOWER_NON_CRITICAL_GOING_LOW  0x1
#define LOWER_NON_CRITICAL_GOING_HI   0x2
#define LOWER_CRITICAL_GOING_LOW      0x4
#define LOWER_CRITICAL_GOING_HI       0x8
#define LOWER_NON_RECOVER_GOING_LOW   0x10
#define LOWER_NON_RECOVER_GOING_HI    0x20
#define UPPER_NON_CRITICAL_GOING_LOW  0x40
#define UPPER_NON_CRITICAL_GOING_HI   0x80
#define UPPER_CRITICAL_GOING_LOW      0x100
#define UPPER_CRITICAL_GOING_HI       0x200
#define UPPER_NON_RECOVER_GOING_LOW   0x400
#define UPPER_NON_RECOVER_GOING_HI    0x800

//
// Server Management COM Addressing types
//
typedef enum {
  EfiSmReserved,
  EfiSmIpmb,
  EfiSmIcmb1_0,
  EfiSmIcmb0_9,
  EfiSm802_3_Lan,
  EfiSmRs_232,
  EfiSmOtherLan,
  EfiSmPciSmBus,
  EfiSmSmBus1_0,
  EfiSmSmBus2_0,
  EfiSmUsb1_x,
  EfiSmUsb2_x,
  EfiSmBmc
} EFI_SM_CHANNEL_MEDIA_TYPE;

typedef enum {
  EfiSmTcp,
  EfiSmUdp,
  EfiSmIcmp,
  EfiSmIpmi
} EFI_SM_PROTOCOL_TYPE;

typedef enum {
  EfiSmMessage,
  EfiSmRawData
} EFI_SM_DATA_TYPE;

typedef struct {
  BOOLEAN IpAddressType;
  UINT16  IpPort;
  UINT8   IpAddress[16];
} EFI_SM_IP_ADDRESS;

typedef struct {
  UINT8 SlaveAddress;
  UINT8 LunAddress;
  UINT8 NetFunction;
  UINT8 ChannelAddress;
} EFI_SM_IPMI_ADDRESS;

typedef struct {
  UINT8 SerialPortNumber;
} EFI_SM_SERIAL_ADDRESS;

typedef union {
  EFI_SM_IP_ADDRESS     IpAddress;
  EFI_SM_IPMI_ADDRESS   BmcAddress;
  EFI_SM_SERIAL_ADDRESS SerialAddress;
} EFI_SM_COM_ADDRESS_TYPE;

typedef struct {
  EFI_SM_CHANNEL_MEDIA_TYPE ChannelType;
  EFI_SM_COM_ADDRESS_TYPE   Address;
} EFI_SM_COM_ADDRESS;

#pragma pack(1)
//
// Sensor Reading Data
//
typedef enum {
  DataLinear,                                   // Linear
  DataNaturalLog,                               // Ln(x)
  DataLog10,                                    // Log10(x)
  DataLog2,                                     // Log2(x)
  Datae,                                        // e
  DataExp10,                                    // Exp 10
  DataExp2,                                     // Exp 2
  DataInverse,                                  // 1/x
  DataSqr,                                      // Sqr
  DataCube,                                     // Cube
  DataSqrt,                                     // Square Root
  DataCubeInverse                               // Cube-1 (x)
} LINERIZATION_TYPE;

typedef union {
  UINT8   SensorUint8Data[2];
  UINT16  SensorUint16Data;
} SENSOR_SPLIT_DATA;

typedef struct {
  LINERIZATION_TYPE Linearization;              // L
  UINT8             Tolerance;                  // Tolerance
  UINT8             AdditiveOffsetExp;          // k1
  UINT8             AccuracyExp;                // Accuracy Exponential
  UINT8             ResultExponent;             // k2
  SENSOR_SPLIT_DATA IntegerConstantMultiplier;  // M
  SENSOR_SPLIT_DATA AdditiveOffset;             // B
  SENSOR_SPLIT_DATA Accuracy;                   // Accuracy
} EFI_SENSOR_CONVERSION_DATA;

//
// Server Management Controller Information
//
typedef struct {
  UINT8   DeviceId;
  UINT8   DeviceRevision : 4;
  UINT8   Reserved : 3;
  UINT8   DeviceSdr : 1;
  UINT8   MajorFirmwareRev : 7;
  UINT8   UpdateMode : 1;
  UINT8   MinorFirmwareRev;
  UINT8   SpecificationVersion;
  UINT8   SensorDeviceSupport : 1;
  UINT8   SdrRepositorySupport : 1;
  UINT8   SelDeviceSupport : 1;
  UINT8   FruInventorySupport : 1;
  UINT8   IPMBMessageReceiver : 1;
  UINT8   IPMBMessageGenerator : 1;
  UINT8   BridgeSupport : 1;
  UINT8   ChassisSupport : 1;
  UINT8   ManufacturerId[3];
  UINT16  ProductId;
  UINT32  AuxFirmwareRevInfo;
} EFI_SM_CTRL_INFO;

#pragma pack()

typedef enum {
  Unicode,
  BcdPlus,
  Ascii6BitPacked,
  AsciiLatin1
} SENSOR_ID_STRING_TYPE;

//
// SENSOR Structures
//
typedef struct {
  BOOLEAN                     Valid;              // Data is Valid
  EFI_SENSOR_CONVERSION_DATA  ConversionParam;    // Conversion Parameters
  UINT8                       UpperNonRec;        // Upper Non Recoverable
  UINT8                       UpperCritical;      // Upper Critical
  UINT8                       UpperNonCritical;   // Upper Non Critical
  UINT8                       LowerNonRec;        // Lower Non Recoverable
  UINT8                       LowerCritical;      // Lower Critical
  UINT8                       LowerNonCritical;   // Lower Non Critical
} EFI_SENSOR_THRESHOLD_STRUCT;

typedef struct {
  BOOLEAN                     Valid;              // Structure Valid
  EFI_SENSOR_CONVERSION_DATA  ConversionParam;    // Conversion Parameters
  SENSOR_ID_STRING_TYPE       SensorIdStringType; // Sensor ID String type
  UINT8                       NominalReading;     // Nominal Reading of the Sensor
  UINT8                       SensorId[16];       // Sensor Description
} EFI_SENSOR_READING_STRUCT;

//
//  IPMI HOB
//
typedef struct {
  UINT16  IoBasePort;
} IPMI_HOB_DATA;

//
// COM Layer Callback
//
typedef
EFI_STATUS
(EFIAPI *EFI_SM_CALLBACK_PROC) (
  OUT  EFI_STATUS                          Status,
  IN  VOID                                 *UserContext
  );

typedef struct {
  EFI_SM_CALLBACK_PROC  SmCallback;
  VOID                  *UserContext;
} EFI_SM_CALLBACK;

#endif
