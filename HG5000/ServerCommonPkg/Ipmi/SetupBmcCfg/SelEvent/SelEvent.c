/*++

Copyright (c) 2019, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:

Abstract:
  BMC System log.

Revision History:

--*/
#include <Uefi.h>
#include <Guid/MdeModuleHii.h>
#include <Library/PcdLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/DevicePathLib.h>
#include <Library/HiiLib.h>
#include <Library/HobLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PrintLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Protocol/HiiString.h>
#include <Protocol/HiiDatabase.h>
#include <Protocol/HiiConfigRouting.h>
#include <Protocol/HiiConfigAccess.h>
#include <SetupBmcCfg.h>
#include <IndustryStandard/IpmiNetFnStorage.h>
#include <Protocol/Smbios.h>
#include <IndustryStandard/Smbios.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/IpmiBaseLib.h>
#include <IpmiSensorEventData.h>


EFI_STATUS
GetSelInfo (
  IN OUT UINT16          *TotalEntries
  )
{
  EFI_STATUS             Status;
  UINT8                  ResponseBuff[34];
  UINT8                  ResponseSize;
  UINT16                 Entries;
  
  //
  //Get Sel Entries.
  //
  ResponseSize = sizeof (ResponseBuff);
  SetMem (ResponseBuff, ResponseSize, 0);
  Status = EfiSendCommandToBMC (
             EFI_SM_NETFN_STORAGE,
             EFI_STORAGE_GET_SEL_INFO,
             NULL,
             0,
             (UINT8 *)&ResponseBuff,
             (UINT8 *)&ResponseSize
             );
  if (EFI_ERROR (Status)) {
    return EFI_DEVICE_ERROR;
  }

  Entries  = ResponseBuff[1] + (ResponseBuff[2] << 8);
  DEBUG ((EFI_D_INFO, "Sel Entries are %d\n", Entries));
  *TotalEntries = Entries;

  if (0 == Entries) {
    return EFI_SUCCESS;
  }

  if (ResponseBuff[13] & 0x02) {
    //
    //ReserveSel is supported, then issue this cmd.
    //
    ResponseSize = sizeof (ResponseBuff);
    SetMem (ResponseBuff, ResponseSize, 0);
    Status = EfiSendCommandToBMC (
               EFI_SM_NETFN_STORAGE,
               EFI_STORAGE_RESERVE_SEL,
               NULL,
               0,
               (UINT8 *)&ResponseBuff,
               (UINT8 *)&ResponseSize
               );
               
  }

  return EFI_SUCCESS;
}

EFI_STATUS
IpmiGetSelEntries (
  IN UINT16              SelRecordId,
  IN OUT UINT16          *Counts,
  IN OUT SEL_RECORD      *NewSelData,
  IN OUT UINT16          *LastRecordId
  );

EFI_STATUS
ConvertSensorEventLogInfo (
  IN SEL_RECORD          *SelEntry,
  IN OUT CHAR16          **PromptSting,
  IN OUT CHAR16          **HelpSting
  );

EFI_STATUS
IpmiGetSelEntries (
  IN UINT16              SelRecordId,
  IN OUT UINT16          *Counts,
  IN OUT SEL_RECORD      *NewSelData,
  IN OUT UINT16          *LastRecordId
  )
{
  EFI_STATUS             Status;
  UINT8                  CmdDataBuff[10];
  UINT8                  CmdDataSize;
  UINT8                  ResponseBuff[34];
  UINT8                  ResponseSize;
  UINT16                 NextSelRecordId;
  SEL_RECORD             *Ptr;
  UINT16                 Index;
  
  Index = 0;
  NextSelRecordId = SelRecordId;
  CmdDataSize = sizeof (CmdDataBuff);
  SetMem (CmdDataBuff, CmdDataSize, 0);
  CmdDataBuff [5] = 0xff;
  Ptr = NewSelData;

  DEBUG ((EFI_D_ERROR, "IpmiGetSelEntries, Counts :%d.\n", *Counts));
  while ((0xFFFF != NextSelRecordId) && (Index < *Counts)) {
    //
    //Get Sel Entry
    //
    CmdDataBuff [2] = (UINT8)(NextSelRecordId & 0xff);
    CmdDataBuff [3] = (UINT8)((NextSelRecordId >> 8) & 0xff);

    ResponseSize = sizeof (ResponseBuff);
    SetMem (ResponseBuff, ResponseSize, 0);

    Status = EfiSendCommandToBMC (
               EFI_SM_NETFN_STORAGE,
               EFI_STORAGE_GET_SEL_ENTRY,
               (UINT8 *)&CmdDataBuff,
               6,
               (UINT8 *)&ResponseBuff,
               (UINT8 *)&ResponseSize
               );
    if (EFI_ERROR (Status)) {
      DEBUG ((EFI_D_INFO, "Stop getting Sel Entry %x (%r)\n", NextSelRecordId, Status));
      break;
    }

    DEBUG ((EFI_D_ERROR, "IpmiGetSelEntries %d :%d.\n", Index, NextSelRecordId));
    NextSelRecordId = ((ResponseBuff[1] << 8) | ResponseBuff[0]);
    if (0xFFFF != NextSelRecordId) {
      *LastRecordId = NextSelRecordId;
    }
    CopyMem (Ptr, (UINT8 *)(&(ResponseBuff[2])), sizeof (SEL_RECORD));

    Ptr++;
    Index++;
  }
  *Counts = Index;
  return EFI_SUCCESS;
}


EFI_STATUS
ClearBmcSelForm (
  IN  EFI_HII_HANDLE    HiiHandle
  )
{
  EFI_GUID    FormsetGuid = SETUP_BMC_CFG_GUID;
  VOID    *StartOpCodeHandle;
  EFI_IFR_GUID_LABEL    *StartLabel;
  VOID    *EndOpCodeHandle;
  EFI_IFR_GUID_LABEL    *EndLabel;
  EFI_STRING_ID    Prompt;
  EFI_STRING_ID    Help;

  HiiSetString (HiiHandle, STRING_TOKEN (STR_SEL_TOTALS_VALUE), L" ", NULL);

  StartOpCodeHandle = HiiAllocateOpCodeHandle ();
  EndOpCodeHandle  = HiiAllocateOpCodeHandle ();
  if ((NULL == StartOpCodeHandle) || (NULL == EndOpCodeHandle)) {
    return EFI_OUT_OF_RESOURCES;
  }
  StartLabel = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (StartOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
  StartLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  StartLabel->Number       = LABEL_SEL_START;
  EndLabel = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (EndOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
  EndLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  EndLabel->Number       = LABEL_SEL_END;

  Prompt = HiiSetString(HiiHandle, 0, L" ", NULL);
  Help = HiiSetString(HiiHandle, 0, L" ", NULL);
  HiiCreateTextOpCode (StartOpCodeHandle, Prompt, Help, 0);

  HiiUpdateForm (
      HiiHandle,
      &FormsetGuid,
      SEL_FORM_ID,
      StartOpCodeHandle,
      EndOpCodeHandle
      );

  HiiFreeOpCodeHandle (StartOpCodeHandle);
  HiiFreeOpCodeHandle (EndOpCodeHandle);

  return EFI_SUCCESS;
}

#define MAX_SEL_COUNT    200

EFI_STATUS
UpdateBmcSelForm (
  IN  EFI_HII_HANDLE    HiiHandle
  )
{
  EFI_STATUS    Status;
  CHAR16    TempString[100];
  //EFI_IPMI_TRANSPORT    *IpmiTransport = NULL;
  UINT16    TotalSel;
  SEL_RECORD    *LogArray;
  SEL_RECORD    *Ptr = NULL;
  UINT16    StartEntry = 0xFFFF;
  UINT16    LastRecordId = 0;
  UINT16    Counts;
  UINT16    Index;
  EFI_GUID    FormsetGuid = SETUP_BMC_CFG_GUID;
  VOID    *StartOpCodeHandle;
  EFI_IFR_GUID_LABEL    *StartLabel;
  VOID    *EndOpCodeHandle;
  EFI_IFR_GUID_LABEL    *EndLabel;
  CHAR16    *PromptSting;
  CHAR16    *HelpSting;
  EFI_STRING_ID    Prompt = 0;
  EFI_STRING_ID    Help   = 0;

  //
  // Get total number.
  //
  TotalSel = 0;
  Status  = GetSelInfo (&TotalSel);
  DEBUG ((EFI_D_ERROR, "UpdateBmcSelForm, GetSelInfo :%r, TotalSel :%d..\n", Status, TotalSel));
  if (!TotalSel) {
    return EFI_NOT_FOUND;
  }

  Counts = TotalSel;
  StartEntry = 0;
  if (Counts > MAX_SEL_COUNT) {
    Counts = MAX_SEL_COUNT;
    StartEntry = TotalSel  - MAX_SEL_COUNT+1;
    TotalSel = MAX_SEL_COUNT;
  }
  
  ZeroMem (TempString, sizeof (TempString));
  UnicodeSPrint (TempString, sizeof (TempString), L"%d", TotalSel);
  HiiSetString (HiiHandle, STRING_TOKEN (STR_SEL_TOTALS_VALUE), TempString, NULL);
  //
  //Get Sel entries.
  //
  LogArray = NULL;
  LogArray = (SEL_RECORD*) AllocateZeroPool(sizeof(SEL_RECORD) * TotalSel);
  if (NULL == LogArray) {
    DEBUG ((EFI_D_ERROR, "UpdateBmcSelForm, No enough memory resource\n"));
    return EFI_OUT_OF_RESOURCES;
  }

  Ptr = LogArray;  
  IpmiGetSelEntries (StartEntry, &Counts, Ptr, &LastRecordId);
  DEBUG ((EFI_D_ERROR, "UpdateBmcSelForm, LastRecordId :0x%x, Counts :%d.\n", LastRecordId, Counts));

  //
  // Update to form.
  //
  //
  StartOpCodeHandle = HiiAllocateOpCodeHandle ();
  EndOpCodeHandle  = HiiAllocateOpCodeHandle ();
  if ((NULL == StartOpCodeHandle) || (NULL == EndOpCodeHandle)) {
    FreePool (LogArray);
    return EFI_OUT_OF_RESOURCES;
  }
  StartLabel = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (StartOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
  StartLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  StartLabel->Number       = LABEL_SEL_START;
  EndLabel = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (EndOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
  EndLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  EndLabel->Number       = LABEL_SEL_END;

  Index = TotalSel - 1;
  while (1) {
    ConvertSensorEventLogInfo (&(LogArray[Index]), &PromptSting, &HelpSting);
    //DEBUG ((EFI_D_ERROR, "UpdateBmcSelForm, PromptSting :%s.\n", PromptSting));
    if (NULL != PromptSting) {
      Prompt = HiiSetString(HiiHandle, 0, PromptSting, NULL);
      FreePool (PromptSting);
      PromptSting = NULL;
    }
    if (NULL != HelpSting) {
      Help = HiiSetString(HiiHandle, 0, HelpSting, NULL);
      FreePool (HelpSting);
      HelpSting = NULL;
    }
    HiiCreateTextOpCode (StartOpCodeHandle, Prompt, Help, 0);

    if (0 == Index) {
      break;
    }
    Index--;
  }

  HiiUpdateForm (
      HiiHandle,
      &FormsetGuid,
      SEL_FORM_ID,
      StartOpCodeHandle,
      EndOpCodeHandle
      );

  HiiFreeOpCodeHandle (StartOpCodeHandle);
  HiiFreeOpCodeHandle (EndOpCodeHandle);
  FreePool (LogArray);
  
  return EFI_SUCCESS;
}


