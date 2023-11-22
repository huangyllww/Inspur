/** @file

Copyright (c) 2006 - 2021, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  TcmSetupInfoReport.c

Abstract:
  Report TCM chip information to setup option.

Revision History:

**/


#include <Library/BaseLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/TcmCommLib.h>
#include <Library/PrintLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
// #include <Protocol/TcmService.h>
#include "TcmSetup.h"


#define MAX_VALUE_LENGTH                         0x30

#define MANU_ID_NTZ                              0x005A544E

#pragma pack(1)

typedef struct {
  UINT32  Manu_ID;
  CHAR8*  Manu_Name;
} MANU_NAME_INFO;

#pragma pack()

MANU_NAME_INFO TpmManuName[] = {
  {MANU_ID_NTZ, "NTZ"},
  {0x0,         "N/A"}
};

VOID
ReportTcm12Info (
  IN TCM_SETUP_PRIVATE_DATA  *TcmSetupPrivateData
  )
{
  EFI_STATUS                         Status;
  UINT32                             Manufacture;
  UINT8*                             TempStr;
  UINT8                              ManuIndex;
  EFI_TCM_HANDLE                     TcmHandle;
  TCM_VERSION                        TcmFirmwareVer;
  CHAR16                             ManufactureStr[16];
  CHAR16                             VersionStr[16];

  TempStr = NULL;
  ManuIndex = 0;
  TcmHandle = (EFI_TCM_HANDLE)(UINTN)PcdGet64 (PcdTpmBaseAddress);

  ZeroMem (&TcmFirmwareVer, sizeof (TCM_VERSION));
  Status = gBS->AllocatePool (EfiBootServicesData, (MAX_VALUE_LENGTH), (VOID**)&TempStr);
  if (Status != EFI_SUCCESS) {
    return ;
  }

  //
  // Manufacture string
  //
  Status = TcmGetCapabilityManufactureID (TcmHandle, &Manufacture);
  DEBUG ((EFI_D_INFO, "[ReportTcm12Info] TCM ManufactureID:(%x) \n", Manufacture));

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "TcmSetup: get TCM ManufactureID failed!"));
    HiiSetString (TcmSetupPrivateData->HiiHandle, STRING_TOKEN (STR_TCM_DEVICE_FIRMWARE_VERSION_CONTENT), L"Unknown", NULL);
  } else {
    for (ManuIndex = 0; ManuIndex < sizeof(TpmManuName) / sizeof(MANU_NAME_INFO); ManuIndex++) {
      if (Manufacture == TpmManuName[ManuIndex].Manu_ID) {
        DEBUG ((EFI_D_INFO, "[ReportTcm12Info] TCM ManufactureID:(%8x) \n", TpmManuName[ManuIndex].Manu_Name));
        AsciiStrToUnicodeStrS ((CHAR8 *)TpmManuName[ManuIndex].Manu_Name, ManufactureStr, 16);
        HiiSetString (TcmSetupPrivateData->HiiHandle, STRING_TOKEN (STR_TCM_DEVICE_VENDOR_CONTENT), ManufactureStr, NULL);
        break;
      }
    }
  }

  //
  // Firmware Version string
  //
  Status = TcmGetCapabilityFirmwareVersion (TcmHandle, &TcmFirmwareVer);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "TcmSetup: get TCM FirmwareVersion failed!"));
    HiiSetString (TcmSetupPrivateData->HiiHandle, STRING_TOKEN (STR_TCM_DEVICE_FIRMWARE_VERSION_CONTENT), L"Unknown", NULL);
  } else {
    AsciiSPrint ((CHAR8 *)TempStr, MAX_VALUE_LENGTH, "%d.%d", TcmFirmwareVer.major, TcmFirmwareVer.minor);
    AsciiStrToUnicodeStrS ((CHAR8 *)TempStr, VersionStr, 16);
    HiiSetString (TcmSetupPrivateData->HiiHandle, STRING_TOKEN (STR_TCM_DEVICE_FIRMWARE_VERSION_CONTENT), VersionStr, NULL);
  }

  if (NULL != TempStr) {
    gBS->FreePool (TempStr);
  }

  return;
}