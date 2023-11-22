/** @file

Copyright (c) 2021, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  ReportTPMInfoToSetup.c

Abstract:
  Report TPM Information to Setup.

Revision History:

TIME:
$AUTHOR:
$REVIEWERS:
$SCOPE:
$TECHNICAL:


T.O.D.O

$END--------------------------------------------------------------------------------------------------------------------

**/

#include "Tcg2ConfigImpl.h"
#include "ReportTPMInfoToSetup.h"
#include <Library/Tpm2CommandLib.h>
#include <Guid/Tpm2DeviceInfo.h>


MANU_NAME_INFO TpmManuName[] = {
  {MANU_ID_INTC,"INTC"},
  {MANU_ID_NTZ, "NTZ"},
  {MANU_ID_IFX, "IFX"},
  {MANU_ID_STM, "STM"},
  {MANU_ID_HYG, "HYG"},
  {MANU_ID_NTC, "NTC"},
  {0x0,         "N/A"}
};


VOID
ReportTpm2Info (
  IN TCG2_CONFIG_PRIVATE_DATA   *PrivateData
  )
{
  EFI_STATUS                         Status;
  UINT32                             FirmwareMax;
  UINT32                             Manufacture;
  CHAR8*                             TempStr;
  UINT8                              ManuIndex;
  CHAR16                             ManufactureStr[16];
  CHAR16                             VersionStr[16];
  TPM2_DEVICE_INFO                   GetTpm2DeviceInfo;
  UINTN                              Tpm2DeviceInfoDataSize;

  TempStr = NULL;
  ManuIndex = 0;

  Status = gBS->AllocatePool (EfiBootServicesData, (MAX_VALUE_LENGTH), (VOID**)&TempStr);
  if (Status != EFI_SUCCESS) {
    return ;
  }

  Tpm2DeviceInfoDataSize = sizeof (GetTpm2DeviceInfo);
  Status = gRT->GetVariable (
                  TPM2_DEVICE_INFO_NAME,
                  &gTpm2DeviceInfoGuid,
                  NULL,
                  &Tpm2DeviceInfoDataSize,
                  &GetTpm2DeviceInfo
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Tpm2Setup: Fail to get TPM2_DEVICE_INFO_NAME\n"));
    //
    // Manufacture string
    //
    HiiSetString (PrivateData->HiiHandle, STRING_TOKEN (STR_TCG2_DEVICE_FIRMWARE_VERSION_CONTENT), L"Unknown", NULL);
    //
    // Firmware Version string
    //
    HiiSetString (PrivateData->HiiHandle, STRING_TOKEN (STR_TCG2_DEVICE_FIRMWARE_VERSION_CONTENT), L"Unknown", NULL);
  } else {
    //
    //Manufacture string
    //
    Manufacture = GetTpm2DeviceInfo.ManufactureId;
    for (ManuIndex = 0; ManuIndex < sizeof(TpmManuName) / sizeof(MANU_NAME_INFO); ManuIndex++) {
      if (Manufacture == TpmManuName[ManuIndex].Manu_ID) {
        AsciiStrToUnicodeStrS ((CHAR8 *)TpmManuName[ManuIndex].Manu_Name, ManufactureStr, 16);
        HiiSetString (PrivateData->HiiHandle, STRING_TOKEN (STR_TCG2_DEVICE_VENDOR_CONTENT), ManufactureStr, NULL);
        break;
      }
    }

    //
    // Firmware Version string
    //
    FirmwareMax = GetTpm2DeviceInfo.FirmwareVersion1;
    AsciiSPrint (TempStr, MAX_VALUE_LENGTH, "%d.%d", (FirmwareMax & 0xFFFF0000) >> 16, FirmwareMax & 0xFFFF);
    AsciiStrToUnicodeStrS (TempStr, VersionStr, 16);
    HiiSetString (PrivateData->HiiHandle, STRING_TOKEN (STR_TCG2_DEVICE_FIRMWARE_VERSION_CONTENT), VersionStr, NULL);
  }

  if (NULL != TempStr) {
    gBS->FreePool (TempStr);
  }

}

