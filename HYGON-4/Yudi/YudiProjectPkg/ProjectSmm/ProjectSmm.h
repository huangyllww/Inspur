/** @file

Copyright (c) 2006 - 2019, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  CrbSmmPlatform.c

Abstract:
  Source file for the Smm platform driver.

Revision History:

**/

#ifndef __PROJECT_SMM_H__
#define __PROJECT_SMM_H__

#include <PiSmm.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Library/IoLib.h>
#include <Library/PciLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/HobLib.h>
#include <Library/UefiLib.h>
#include <Library/ReportStatusCodeLib.h>
#include <Library/ByoRtcLib.h>
#include <Protocol/SmmCpu.h>
#include <Protocol/SmmPowerButtonDispatch2.h>
#include <Protocol/SmmSxDispatch2.h>
#include <Protocol/SmmSwDispatch2.h>
#include <Protocol/SmmPowerButtonDispatch2.h>
#include <Library/PlatformCommLib.h>
#include <Library/PlatformCommLib.h>
#include <Library/ByoCommLib.h>
#include <SetupVariable.h>
#include <Protocol/ByoCommSmiSvcProtocol.h>
#include <Library/ByoHygonCommLib.h>


extern UINT16         mAcpiBaseAddr;
extern SETUP_DATA     gSetupData;
extern BOOLEAN        gSmmReadyToBoot;




EFI_STATUS GetCurrentTime(EFI_TIME *Time);
EFI_STATUS SetCurrentTime(EFI_TIME *Time, UINTN SetFlag);
EFI_STATUS UsbFilterInit();
EFI_STATUS EFIAPI GenerateUUID(EFI_GUID *Guid);
EFI_STATUS EFIAPI GenerateUUIDBySn(CHAR8 *Sn, EFI_GUID *Guid);

EFI_STATUS
ByoLegacyUsbFilter (
  IN UINTN                             DevType,
  IN EFI_DEVICE_PATH_PROTOCOL          *DevPath
  );

EFI_STATUS
ByoLegacyUsbFilter2 (
  IN EFI_DEVICE_PATH_PROTOCOL          *DevPath
  );

EFI_STATUS ProjectEnableS5RtcWake();



#endif

