/** @file

Copyright (c) 2006 - 2019, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

By: 
  winddy_zhang

File Name:

Abstract:

Revision History:

$END----------------------------------------------------------------------------
**/

#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/HardwareMonitor.h>
#include <Ite8625Reg.h>
#include <Library/SuperIoLib.h>


EFI_STATUS
GetSensorData (
  IN  SENSOR_SELECT      Type,
  OUT UINTN              *Value
  )
{
  EFI_STATUS  Status;
  UINT16      Data16 = 0;
  

  Status = EFI_SUCCESS;
  
  switch (Type) {

    case SENSOR_CPU_FAN:
      do {
        Data16 = (ReadHwmReg(R_FAN_TAC1_EXT_READING) << 8) + ReadHwmReg(R_FAN_TAC1_READING);
        if (Data16 != 0){break;}
        gBS->Stall(100);
      } while (TRUE);
      if (Data16 == 0xFFFF) {
        *Value = 0;
      } else {
        *Value = 1350000/(Data16 * 2);
      }
      break;

    case SENSOR_SYS_FAN:
      do {
        Data16 = (ReadHwmReg(R_FAN_TAC2_EXT_READING) << 8) + ReadHwmReg(R_FAN_TAC2_READING);
        if (Data16 != 0){break;}
        gBS->Stall(100);
      } while (TRUE);
      if (Data16 == 0xFFFF) {
        *Value = 0;
      } else {
        *Value = 1350000/(Data16 * 2);
      }
      break;

    case SENSOR_TEMPIN1:
      *Value = ReadHwmReg(R_TMPIN1_READING);
      break;

    case SENSOR_TEMPIN2:
      *Value = ReadHwmReg(R_TMPIN2_READING);
      break;      
      
    case SENSOR_TEMPIN3:
      *Value = ReadHwmReg(R_TMPIN3_READING);
      break;   

    case SENSOR_VIN0:
      *Value = ReadHwmReg(R_VIN0_READING) * 109;
      break;

    case SENSOR_VIN1:
      *Value = ReadHwmReg(R_VIN1_READING) * 109;
      break;

    case SENSOR_VIN2:
      *Value = ReadHwmReg(R_VIN2_READING) * 109;
      break;

    case SENSOR_VIN3:
      *Value = ReadHwmReg(R_VIN3_READING) * 109;
      break;

    case SENSOR_VIN4:
      *Value = ReadHwmReg(R_VIN4_READING) * 109;
      break;

    case SENSOR_VIN5:
      *Value = ReadHwmReg(R_VIN5_READING) * 109;
      break;

    case SENSOR_VIN6:
      *Value = ReadHwmReg(R_VIN6_READING) * 109;
      break;

    case SENSOR_VBAT:
      *Value = ReadHwmReg(R_VBAT_READING) * 109 * 2;
      break;
      
    default:
      Status = EFI_INVALID_PARAMETER;
      break;
  }

  return Status;
}


EFI_SIO_HWM_PROTOCOL mSioHwm = {
  GetSensorData
};


EFI_STATUS
SioDxeEntry (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS                    Status;
//UINTN                         Speed;


  DEBUG((EFI_D_INFO, "%a\n", __FUNCTION__));

  Status = gBS->InstallMultipleProtocolInterfaces (
                  &gImageHandle,
                  &gEfiSioHwmProtocolGuid,
                  &mSioHwm,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);

//mSioHwm.GetSensorData(SENSOR_CPU_FAN, &Speed);
//DEBUG((EFI_D_INFO, "Fan:%d\n", Speed));

  return Status;
}

