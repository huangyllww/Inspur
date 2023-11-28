/** @file

Copyright (c) 2006 - 2016, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  KcsBmc.h

Abstract:
  Source file for the IPMI driver.

Revision History:

**/


#ifndef _SM_SDR_READING_DRIVER_H_
#define _SM_SDR_READING_DRIVER_H_

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>

#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>

#include "EfiServerManagement.h"

#include <Library/IpmiBaseLib.h>
#include <Protocol/IpmiInterfaceProtocol.h>



#define SDR_SENSOR_DEVICE_PROTOCOL_GUID \
  { \
    0x4c8ca0dd, 0x8de2, 0x44f7, { 0x8e, 0x1e, 0xc2, 0x56, 0x44, 0x42, 0x5, 0xba } \
  }

#define SDR_MAX_READ_LEN        20
#define READING_UNAVAILABLE     0x20
#define SCANNING_DISABLED       0x40
#define EVENT_MSG_DISABLED      0x80

#define SDR_SENSOR_L_LINEAR     0x00
#define SDR_SENSOR_L_LN         0x01
#define SDR_SENSOR_L_LOG10      0x02
#define SDR_SENSOR_L_LOG2       0x03
#define SDR_SENSOR_L_E          0x04
#define SDR_SENSOR_L_EXP10      0x05
#define SDR_SENSOR_L_EXP2       0x06
#define SDR_SENSOR_L_1_X        0x07
#define SDR_SENSOR_L_SQR        0x08
#define SDR_SENSOR_L_CUBE       0x09
#define SDR_SENSOR_L_SQRT       0x0a
#define SDR_SENSOR_L_CUBERT     0x0b
#define SDR_SENSOR_L_NONLINEAR  0x70



/* SDR Record Common Sensor header macros */
#define IS_THRESHOLD_SENSOR(s)    ((s)->EventType == 1)
#define UNITS_ARE_DISCRETE(s)     (((s)->SensorUnits1 & 0xC0) == 0xC0)

#define IS_READING_UNAVAILABLE(val)    ((val) & READING_UNAVAILABLE)
#define IS_SCANNING_DISABLED(val)      (!((val) & SCANNING_DISABLED))
#define IS_EVENT_MSG_DISABLED(val)     (!((val) & EVENT_MSG_DISABLED))

#define tos32(val, bits)    ((val & ((1<<((bits)-1)))) ? (-((val) & (1<<((bits)-1))) | (val)) : (val))

#define BSWAP_16(x) ((((x) & 0xff00) >> 8) | (((x) & 0x00ff) << 8))
#define BSWAP_32(x) ((((x) & 0xff000000) >> 24) | (((x) & 0x00ff0000) >> 8) |\
                    (((x) & 0x0000ff00) << 8) | (((x) & 0x000000ff) << 24))

#define __TO_TOL(mtol)     (UINT16)(BSWAP_16(mtol) & 0x3f)
#define __TO_M(mtol)       (INT16)(tos32((((BSWAP_16(mtol) & 0xff00) >> 8) | ((BSWAP_16(mtol) & 0xc0) << 2)), 10))
#define __TO_B(bacc)       (INT32)(tos32((((BSWAP_32(bacc) & 0xff000000) >> 24) | \
                           ((BSWAP_32(bacc) & 0xc00000) >> 14)), 10))
#define __TO_ACC(bacc)     (UINT32)(((BSWAP_32(bacc) & 0x3f0000) >> 16) | ((BSWAP_32(bacc) & 0xf000) >> 6))
#define __TO_ACC_EXP(bacc) (UINT32)((BSWAP_32(bacc) & 0xc00) >> 10)
#define __TO_R_EXP(bacc)   (INT32)(tos32(((BSWAP_32(bacc) & 0xf0) >> 4), 4))
#define __TO_B_EXP(bacc)   (INT32)(tos32((BSWAP_32(bacc) & 0xf), 4))



typedef struct _SDR_SENOSR_DEV_PROTOCOL SDR_SENOSR_DEV_PROTOCOL;

typedef struct  {
    UINT8        s_id[17];                 /* name of the sensor */
    UINT8        s_recordtype;             /* record type*/
    EFI_IPMI_SDR_RECORD_STRUCT_1    *full;
    EFI_IPMI_SDR_RECORD_STRUCT_2    *compact;
    UINT8        s_reading_valid;          /* read value valididity */
    UINT8        s_scanning_disabled;      /* read of value disabled */
    UINT8        s_reading_unavailable;    /* read value unavailable */
    UINT8        s_reading;                /* value which was read */
    UINT8        s_data2;                  /* data2 value read */
    UINT8        s_data3;                  /* data3 value read */
    UINT8        s_has_analog_value;       /* sensor has analog value */
//  double          s_a_val;               /* read value converted to analog */
//  CHAR8           s_a_str[16];           /* analog value as a string */
//  CONST CHAR8    *s_a_units;             /* analog value units string */
}SENSOR_READING;

typedef struct  {
    UINT8        SensorNumber;             /* Number of the sensor */
    UINT16       M;
    UINT16       B;
    UINT8        K1;
    UINT8        K2;
    UINT8        Type;
} OEM_SENSOR_READING_VAL;

typedef
EFI_STATUS
(EFIAPI *EFI_SENSOR_RADING) (
  IN  SDR_SENOSR_DEV_PROTOCOL  *This,
  IN  UINT8                     SensorNum,
  IN  UINT8                     Lun,
  OUT UINTN                    *Value,
  OUT UINT8                    *Div
  );

struct _SDR_SENOSR_DEV_PROTOCOL{
  EFI_SENSOR_RADING  SensorReading; 
} ;


extern EFI_GUID gSdrSensorDevProtocolGuid;
#endif
