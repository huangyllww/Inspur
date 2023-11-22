
/** @file

Copyright (c) 2023, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

**/


#ifndef _SIO_HWM_DRIVER_H_
#define _SIO_HWM_DRIVER_H_

#include <PiDxe.h>
#include <IndustryStandard/Pci.h>
#include <Library/BaseLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/S3BootScriptLib.h>
#include <Library/PciLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DevicePathLib.h>
#include <Library/UefiLib.h>
#include <Library/PcdLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>

//
// Driver Consumed Protocol Prototypes
//
#include <Protocol/DriverBinding.h>
#include <Protocol/HardwareMonitor.h>

#define T_MAX 150
#define PWM_MAX 255
#define START_TMP 52
#define START_PWM 50
#define CRITICALTMP_TOLERANCE 3
#define TOLERANCE 2

#define SLOPE ((PWM_MAX-START_PWM)/(T_MAX-START_TMP))


#define T1 START_TMP
#define T2 (START_TMP+5)
#define T3 (START_TMP+10)
#define T4 T_MAX

#define PWM1 START_PWM
#define PWM2 (START_PWM+5*SLOPE)
#define PWM3 (START_PWM+10*SLOPE)
#define PWM4 PWM_MAX

#define CRITICAL_TEMP  (T_MAX-5)
EFI_STATUS
SioHwmDxeInit (
  VOID
);

EFI_STATUS
GetSensorData (
  IN  SENSOR_SELECT  Type,
  OUT UINTN              *Value
  );

UINT8
Get_Sensor(
  IN UINT8 Offset
);

#endif
