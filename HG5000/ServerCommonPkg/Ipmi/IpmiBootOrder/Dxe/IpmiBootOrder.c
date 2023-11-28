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

//
// Statements that include other files
//
#include  <Protocol/LegacyBios.h>
#include  <IndustryStandard/Pci.h>
#include  <Library/IoLib.h>
#include  "IpmiBoot.h"
#include  "IpmiBootOrder.h"
#include  <Library/UefiLib.h>

#include <Library/UefiBootManagerLib.h>
#include <Library/DevicePathLib.h>
#include <Guid/LegacyDevOrder.h>

//
// --------------------------------------
//
#define KCS_MAX_SIZE (128+0x10)
#define BUFFER_SIZE 50
#define IPMI_STALL  1000

UINT8                     SmParamBlockSize  = 0;
BBS_TABLE                 *BbsTable         = NULL;
BOOLEAN                   gLoadDefault = FALSE;

EFI_DEVICE_PATH_PROTOCOL  EndDevicePath[] = {
  END_DEVICE_PATH_TYPE,
  END_ENTIRE_DEVICE_PATH_SUBTYPE,
  END_DEVICE_PATH_LENGTH,
  0
};

VOID
BuildLegacyDevNameString (
  IN  BBS_TABLE                 *CurBBSEntry,
  IN  UINTN                     Index,
  IN  UINTN                     BufSize,
  OUT CHAR16                    *BootString
  );

void
AsciiToUnicodeSize (
  IN UINT8              *a,
  IN UINTN              Size,
  OUT UINT16            *u
  )
{
  UINTN i;
  i = 0;
  while ((a[i] != 0) && ((i+1) < Size)) {
    u[i] = (CHAR16) a[i];
    i++;
  }
  u[i] = 0;
}

VOID
UnicodeToAsciiSize (
  IN UINT16             *u,
  IN UINTN              Size,
  IN OUT UINT8          *a
  )
{
  UINTN i;

  if (NULL == u || NULL == a || 0 == Size) return;

  i = 0;
  while (u[i] != 0 && (i+1) < Size) {
    a[i] = (CHAR8) u[i];
    i++;
  }
  a[i] = 0;
}

void
SafeFreePool (
  IN VOID    *Buffer
  )
{
  if (Buffer != NULL) {
    gBS->FreePool (Buffer);
  }
}

UINTN
EFIAPI
StrLenLocal (
  IN      CONST CHAR16              *String
  )
{
  UINTN         Length;

  for (Length = 0; *String != L'\0'; String++, Length++);
  return Length;
}

BOOLEAN
BdsIsLegacyBootOption (
  IN  CHAR16                * VariableName,
  OUT BBS_TABLE             **BbsEntry,
  OUT UINT16                *BbsIndex
  )
/*++

Routine Description:
  Check the input boot option is a legacy or not
  
Arguments:
  *Buffer     - Buffer to be free
   
Returns:
  TRUE       - this is a legacy boot option
  FALSE      - this is not a legacy boot option
  
--*/  
{
  UINT8                     *LegDevOrder;
  UINTN                     LegDevOrderSize;
  
  EFI_STATUS                    Status;
  EFI_BOOT_MANAGER_LOAD_OPTION  BootOption;
  UINT16                        VarBbsIndex;

  Status = EfiBootManagerVariableToLoadOption (VariableName, &BootOption);
  if (EFI_ERROR(Status)) {
    *BbsEntry = NULL;
    return FALSE;
  }

  Status = GetVariable2 (
            L"LegacyDevOrder",
            &gEfiLegacyDevOrderVariableGuid,
            &LegDevOrder,
            &LegDevOrderSize
            );
  if (EFI_ERROR(Status)) { 
    *BbsEntry = NULL;
    return FALSE;
  }

  if (BbsTable == NULL) {
    *BbsEntry = NULL;
    return FALSE;
  }

  if ((BootOption.FilePath->Type == BBS_DEVICE_PATH) && (BootOption.FilePath->SubType == BBS_BBS_DP)) {
    VarBbsIndex = *(UINT16 *)(BootOption.OptionalData);

    *BbsIndex = VarBbsIndex;
    *BbsEntry = &BbsTable[VarBbsIndex];
    return TRUE;
  }

  *BbsEntry = NULL;
  return FALSE;
}

BOOLEAN
BdsFindLegacyBootOptionByDevType (
  IN UINT16                 *BootOrder,
  IN UINTN                  BootOptionNum,
  IN UINT16                 DevType,
  OUT UINT32                *Attribute,
  OUT UINT16                *BbsIndex,
  OUT UINTN                 *OptionNumber
  )
/*++

Routine Description:
  Find a legacy boot option by device type
  
Arguments:
  *BootOrder        - Pointer to boot order buff
  BootOptionNum     - Boot option index
 DevType            - Device type
  
   
Returns:
  TRUE       - this is a legacy boot option
  FALSE      - this is not a legacy boot option
  
--*/    
{
  UINTN     Index;
  UINTN     BootOrderIndex;
  UINT16    BootOption[100];
  UINTN     BootOptionSize;
  UINT8     *BootOptionVar;
  BBS_TABLE *BbsEntry;
  BOOLEAN   Found;
  EFI_STATUS    Status;

  BbsEntry  = NULL;
  Found     = FALSE;

  if (NULL == BootOrder) {
    return Found;
  }

  for (BootOrderIndex = 0; BootOrderIndex < BootOptionNum; BootOrderIndex++) {
    Index = (UINTN) BootOrder[BootOrderIndex];
    UnicodeSPrint (BootOption, sizeof (BootOption), L"Boot%04x", Index);
    Status = GetVariable2 (
                BootOption,
                &gEfiGlobalVariableGuid,
                &BootOptionVar,
                &BootOptionSize
                );
    if (EFI_ERROR(Status)) {
      continue;
    }

    if (!BdsIsLegacyBootOption (BootOption, &BbsEntry, BbsIndex)) {
      SafeFreePool (BootOptionVar);
      continue;
    }

    if (BbsEntry->DeviceType != DevType) {
      SafeFreePool (BootOptionVar);
      continue;
    }

    *Attribute    = *(UINT32 *) BootOptionVar;
    *OptionNumber = Index;
    Found         = TRUE;
    SafeFreePool (BootOptionVar);
    break;
  }

  return Found;
}

VOID
BotUpdateChecksum (
  IN UINT8      *Buff,
  IN UINT16     BuffLen,
  IN OUT UINT8  *ChksumByte
  )
/*++

Routine Description:
  Calcaulate the checksum for SmBO
  
Arguments:
  *Buff            - Pointer to SmBo buff
  BuffLen          - Length of the SmBo buff
  *ChksumByte      - Checksum of the SmBo
  
   
Returns:
  None
--*/  
{
  UINT8       Chksum = 0;

  *ChksumByte = 0;
  while (BuffLen--) {
    Chksum = Chksum + (*Buff);
    Buff++;
  }
  *ChksumByte = (~Chksum) + 1;
}

BOOLEAN
BotVerifyChecksum (
  IN UINT8      *Buff,
  IN UINT16     BuffLen
  )
{
  UINT8       Chksum = 0;

  while (BuffLen--) {
    Chksum = Chksum + (*Buff);
    Buff++;
  }
  if (Chksum == 0) return TRUE;

  return FALSE;
}

VOID
BuildLegacyDevNameA (
  IN BBS_TABLE                 *BbsEntry,
  IN UINTN                     BbsIndex,
  IN UINTN                     BufSize,
  IN OUT CHAR8                 *DevString
  )
/*++

  Routine Description:

   Build legacy device name string

  Arguments:

    BbsEntry           - Pointer to Bbs table header.
    BbsIndex           - index of one bbs entry
    BufSize            - Size of the input Buff.
    DevString          - Pointer to the created Device name string.

  Returns:

    None
   
Returns:
  None  
--*/    
  
{
  CHAR16            *BootDesc;

  ASSERT(NULL != DevString);
  ASSERT(0 != BufSize);
 
  BootDesc = AllocateZeroPool (BufSize * sizeof(CHAR16));
  ASSERT(BootDesc != NULL);
  
  BuildLegacyDevNameString (BbsEntry, BbsIndex, BufSize*sizeof(CHAR16), BootDesc);
  UnicodeToAsciiSize (BootDesc, BufSize, DevString);
  SafeFreePool(BootDesc);
}

EFI_STATUS
IpmiWaitSetInProgressClearSm (
  VOID
  )
/*++

Routine Description:
  Wait for IPMI Get System Boot Option Set-in-Progress bit to clear & then
  set the Set-in-Progress bit (param# 0).

Arguments:
  None

Returns:
  EFI_SUCCESS if passed, 
  EFI_TIMEOUT if failed

--*/
{
  EFI_STATUS                        Status;
  UINT8                             Buffer[BUFFER_SIZE];
  UINT8                             ResponseSize;
  UINT32                            Retries;
  GET_BOOT_OPTIONS_REQUEST          *GetRequest;
  GET_BOOT_OPTIONS_RESPONSE         *GetResponse;
  BOOT_OPTIONS_RESPONSE_PARAMETER_0 *Parm0;

  SET_BOOT_OPTIONS_REQUEST          *SetRequest;
  SET_BOOT_OPTIONS_RESPONSE       SetResponse;

  Retries = 10;
  gBS->SetMem(Buffer, BUFFER_SIZE, 0);
  GetRequest = (GET_BOOT_OPTIONS_REQUEST *) &Buffer[0];
  GetRequest->ParameterSelector = 0; // Set in progress parameter
  GetRequest->SetSelector   = 0;
  GetRequest->BlockSelector = 0;
  GetResponse = (GET_BOOT_OPTIONS_RESPONSE *) (Buffer + sizeof (GET_BOOT_OPTIONS_REQUEST));
  ResponseSize = sizeof (GET_BOOT_OPTIONS_RESPONSE);

  //
  // Wait for in-progress bit to clear
  //
  while (Retries > 0) {
    Status = EfiSendCommandToBMC (
              EFI_SM_NETFN_CHASSIS,
              EFI_CHASSIS_GET_SYSTEM_BOOT_OPTIONS,
              (UINT8 *) GetRequest,
              sizeof (GET_BOOT_OPTIONS_REQUEST),
              (UINT8 *) GetResponse,
              &ResponseSize
              );
    if (EFI_ERROR (Status)) {
      return Status;
    }
    //
    // any other checks???
    //
    Parm0 = (BOOT_OPTIONS_RESPONSE_PARAMETER_0 *) GetResponse->ParameterData;
    if (Parm0->SetInProgress == 0) {
      break;
    }

    gBS->Stall (IPMI_STALL);
    --Retries;
  }

  if (Retries == 0) {
    return EFI_TIMEOUT;
  }

  // Now set in-progress
//  Retries = 5;  // no retry
  gBS->SetMem(Buffer, BUFFER_SIZE,0);
  SetRequest = (SET_BOOT_OPTIONS_REQUEST *) &Buffer[0];
  SetRequest->ParameterSelector = 0;      // Set in progress parameter
  SetRequest->MarkParameterInvalid  = 0;  // Set param valid
  SetRequest->ParameterData[0] = 0x01;    // Try to set "Set in progress" bit
  ResponseSize = sizeof (SET_BOOT_OPTIONS_RESPONSE);

  //
  // Attempt to set "set in-progress bit"
  //
//  while (Retries > 0) {
    Status = EfiSendCommandToBMC (
              EFI_SM_NETFN_CHASSIS,
              EFI_CHASSIS_SET_SYSTEM_BOOT_OPTIONS,
              (UINT8 *) SetRequest,
              sizeof (SET_BOOT_OPTIONS_REQUEST),
              (UINT8 *) &SetResponse,
              &ResponseSize
              );
    if (EFI_ERROR (Status)) {
      return Status;
    }
//
//    gBS->Stall (IPMI_STALL);
//    --Retries;
//  }
//
//  if (Retries == 0) {
//    return EFI_TIMEOUT;
//  }

  return EFI_SUCCESS;
}

EFI_STATUS
IpmiClearSetInProgressSm (
  VOID
  )
/*++

Routine Description:
  Clear (free up) System Boot Option Set-in-Progress bit (parm #0)

Arguments:
  None

Returns:
  EFI_SUCCESS if passed, 
  EFI_TIMEOUT if failed

--*/
{
  EFI_STATUS                        Status;
  UINT8                             Buffer[BUFFER_SIZE];
  UINT8                             ResponseSize;
  UINT32                            Retries;
  SET_BOOT_OPTIONS_REQUEST          *SetRequest;
  SET_BOOT_OPTIONS_RESPONSE         SetResponse;

  Retries = 10;
  gBS->SetMem(Buffer, BUFFER_SIZE, 0);
  SetRequest = (SET_BOOT_OPTIONS_REQUEST *) &Buffer[0];
  SetRequest->ParameterSelector = 0;      // Set in progress parameter
  SetRequest->MarkParameterInvalid  = 0;  // Set param valid
  SetRequest->ParameterData[0] = 0x00;    // Clear "Set in progress" bit
  ResponseSize = sizeof (SET_BOOT_OPTIONS_RESPONSE);

  //
  // Attempt to set "set in-progress bit"
  //
  while (Retries > 0) {
    Status = EfiSendCommandToBMC (
              EFI_SM_NETFN_CHASSIS,
              EFI_CHASSIS_SET_SYSTEM_BOOT_OPTIONS,
              (UINT8 *) SetRequest,
              sizeof (SET_BOOT_OPTIONS_REQUEST),
              (UINT8 *) &SetResponse,
              &ResponseSize
              );
    //
    // Check for success (0x00)
    //
    if (!EFI_ERROR (Status)) {
      break;
    }

    gBS->Stall (IPMI_STALL);
    --Retries;
  }

  if (Retries == 0) {
    return EFI_TIMEOUT;
  }

  return EFI_SUCCESS;
}

EFI_STATUS
InitSmParamBlockSize (
  VOID
  )
/*++

Routine Description:
 Init Sm data structure

Arguments:
  None

Returns:
  EFI_SUCCESS if passed, 
  EFI_NOT_FOUND if failed

--*/
  
{
  EFI_STATUS            Status                = EFI_SUCCESS;
  UINT8                 Buffer[KCS_MAX_SIZE];
  UINT8                 SetSel                = 0;
  UINT8                 BlockSel              = 0;
  UINT8                 ParamSize             = 0;
  OEM_PARAM_BLOCK_SIZE  *SmBs;

  for (BlockSel = 0; BlockSel <= 1; BlockSel++ ) {
    SmParamBlockSize = 0;
    Buffer[0] = SetSel;
    Buffer[1] = BlockSel;
    ParamSize = sizeof (OEM_PARAM_BLOCK_SIZE);
    Status = IpmiSmGetBootOptions (OEM_PARAM_SELECTOR_BLOCK_SIZE, Buffer, &ParamSize);
    if (!EFI_ERROR(Status)) {
      break;
    }
  }
  if (!EFI_ERROR(Status)) {
    if (*(UINT32 *)Buffer == SIGNATURE_32('_','P','B','_')) {
      if (BotVerifyChecksum (Buffer, ParamSize)) {
        SmBs = (OEM_PARAM_BLOCK_SIZE *)Buffer; 
        SmParamBlockSize = SmBs->SiBlockSize;
        return EFI_SUCCESS;
      }
    }
  }
  return EFI_NOT_FOUND;
}

EFI_STATUS
ReadSmOemParameter (
  IN OUT OPTIONAL OEM_PARAM_HEADER  *OemData,
  IN OUT UINTN                      *DataSize  
  )
/*++

Routine Description:
 Read Sm data structure from BMC

Arguments:
  *OemData        - Pointer to SmBo
  DataSize        - Size of SmBo

Returns:
  EFI_SUCCESS if passed, 
  EFI_NOT_FOUND if failed

--*/
  
{
  EFI_STATUS    Status;
  UINT16        OemDataSize;
  UINT8         Buffer[KCS_MAX_SIZE];
  UINT8         SetSel = 0;
  UINT8         BlockSel = 1;
  UINT8         ParamSize;
  UINT8         *Ptr;
  
  if (NULL == DataSize) {
    return EFI_INVALID_PARAMETER;
  }

  if (SmParamBlockSize == 0) {
    Status = InitSmParamBlockSize ();
    if (EFI_ERROR (Status)) {
      SmParamBlockSize = 0;
      return EFI_DEVICE_ERROR;
    }
    if (SmParamBlockSize > (KCS_MAX_SIZE-0x10)) {
      SmParamBlockSize = 0;
      return EFI_OUT_OF_RESOURCES;
    }
  }

  //
  // Check if Set-in-Progress bit is clear (free) & take ownership 
  //
  Status = IpmiWaitSetInProgressClearSm ();
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Buffer[0] = SetSel;
  Buffer[1] = BlockSel;
  ParamSize = SmParamBlockSize;
  Status = IpmiSmGetBootOptions (OEM_PARAM_SELECTOR_BOOT_ORDER, Buffer, &ParamSize);
  if (EFI_ERROR(Status)) {
    goto read_smoem_exit;
  }

  OemDataSize = ((OEM_PARAM_HEADER *)Buffer)->Length;
  if (NULL == OemData) {
    *DataSize = (UINTN)OemDataSize;   // return actual parameter size - buffer size required
    Status = EFI_SUCCESS;
    goto read_smoem_exit;
  }

  if (0 == *DataSize) {
    *DataSize = (UINTN)OemDataSize;   // return actual parameter size - buffer size required
    Status = EFI_BUFFER_TOO_SMALL;
    goto read_smoem_exit;

  }

  if (*DataSize < (UINTN)OemDataSize) {     // If input buffer size is small,
    if (*DataSize < ParamSize) {            //  copy as much data as possible from
      ParamSize = (UINT8)*DataSize;         //  1st block & return actual size req.
    }
    CopyMem (OemData, Buffer, ParamSize);
    *DataSize = (UINTN)OemDataSize;   // return actual parameter size - buffer size required
    Status = EFI_BUFFER_TOO_SMALL;
    goto read_smoem_exit;
  }

  *DataSize = (UINTN)OemDataSize;
  Ptr = (UINT8 *)OemData;
  do {
    if (OemDataSize < ParamSize) {
      ParamSize = (UINT8)OemDataSize;
    }
    CopyMem(Ptr, Buffer, ParamSize);
    OemDataSize = OemDataSize - ParamSize;
    Ptr += (UINTN)ParamSize;
    if (0 == OemDataSize) break;

    Buffer[0] = SetSel;
    Buffer[1] = ++BlockSel;
    ParamSize = SmParamBlockSize;     // Read whole block
    Status = IpmiSmGetBootOptions (OEM_PARAM_SELECTOR_BOOT_ORDER, Buffer, &ParamSize);
  } while (!EFI_ERROR(Status));

  *DataSize = *DataSize - (UINTN)OemDataSize;    // return actual bytes read

read_smoem_exit:
  //
  // Clear (free up) Set-in-Progress bit
  //
  IpmiClearSetInProgressSm ();

  return Status;
}

EFI_STATUS
WriteSmOemParameter (
  IN OEM_PARAM_HEADER   *OemData,
  IN UINTN              DataSize
  )
/*++

Routine Description:
 Sent Sm data structure to BMC

Arguments:
  *OemData        - Pointer to SmBo
  DataSize        - Size of SmBo

Returns:
  EFI_SUCCESS if passed, 
  EFI_NOT_FOUND if failed

--*/  
{
  EFI_STATUS    Status;
  UINT16        OemDataSize;
  UINT8         Buffer[KCS_MAX_SIZE];
  UINT8         SetSel = 0;
  UINT8         BlockSel = 1;
  UINT8         ParamSize;
  UINT8         TmpSize;
  UINT8         *Ptr;
  
  if (NULL == OemData) {
    return EFI_INVALID_PARAMETER;
  }

  if (SmParamBlockSize == 0) {
    Status = InitSmParamBlockSize ();
    if (EFI_ERROR (Status)) {
      SmParamBlockSize = 0;
      return EFI_DEVICE_ERROR;
    }
    if (SmParamBlockSize > (KCS_MAX_SIZE-0x10)) {
      SmParamBlockSize = 0;
      return EFI_OUT_OF_RESOURCES;
    }
  }

  //
  // Check if Set-in-Progress bit is clear (free) & take ownership 
  //
  Status = IpmiWaitSetInProgressClearSm ();
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = EFI_SUCCESS;
  OemDataSize = OemData->Length;
  Ptr = (UINT8 *)OemData;
  while (OemDataSize) {
    ParamSize = SmParamBlockSize;
    if (OemDataSize < SmParamBlockSize) {
      ParamSize = (UINT8) OemDataSize;
      // Pad zeros at end of remaining block
      gBS->SetMem (&Buffer[2+ParamSize], SmParamBlockSize-ParamSize, 0);
    }
    Buffer[0] = SetSel;
    Buffer[1] = BlockSel;
    CopyMem (&Buffer[2], Ptr, ParamSize);
    TmpSize = SmParamBlockSize+2;   // Write whole block
    Status = IpmiSmSetBootOptions (OEM_PARAM_SELECTOR_BOOT_ORDER, 1, Buffer, &TmpSize);
    if (EFI_ERROR(Status)) break;

    OemDataSize = OemDataSize - (UINT16)ParamSize;
    Ptr += (UINTN)ParamSize;
    BlockSel++;
  }

  //
  // Clear (free up) Set-in-Progress bit
  //
  IpmiClearSetInProgressSm ();

  return Status;
}


VOID
InitBotHeader(
  IN OUT OEM_BOOT_ORDER_TABLE     *SmBo
  )
/*++

Routine Description:
 Init Sm data structure header

Arguments:
  *SmBo        - Pointer to SmBo  

Returns:
  None
--*/  
{
  SmBo->Header.ParamSign  = EFI_SMBO_BOOT_ORDER_TABLE_SIGN;
  SmBo->Header.Checksum   = 0;
  SmBo->Header.Major      = SM_OEM_PARAM_MAJOR_VER;
  SmBo->Header.Minor      = SM_OEM_PARAM_MINOR_VER;
  SmBo->Header.Length     = sizeof(OEM_BOOT_ORDER_TABLE);
}

EFI_STATUS
FindThisLegacyDevOrder (
  IN BBS_TYPE     DevType,
  IN UINT8        *LegacyDevOrder,
  IN UINT16       LegacyDevOrderSize,
  OUT UINT8       **ThisDevOrder
  )
/*++

Routine Description:
 Find Bbs device order

Arguments:
  DevType           - Device type
  *DevOrder         - Device order of one group
  DevOrderSize      - size of this group
  **LegDevOrder     - Pointer to LegacyDevorder

Returns:
  EFI_SUCCESS if passed, 
  EFI_NOT_FOUND if failed

--*/  
{
  UINT8           *OrigBuffer;

  if (NULL == LegacyDevOrder || NULL == ThisDevOrder) {
    return EFI_INVALID_PARAMETER;
  }

  OrigBuffer = LegacyDevOrder;
  while (LegacyDevOrder < OrigBuffer + LegacyDevOrderSize) {
    if (DevType == *(BBS_TYPE *)LegacyDevOrder) {
      *ThisDevOrder = LegacyDevOrder;
      return EFI_SUCCESS;
    }

    LegacyDevOrder += sizeof (BBS_TYPE);
    LegacyDevOrder += *(UINT16 *) LegacyDevOrder;
  }

  *ThisDevOrder = NULL;
  return EFI_NOT_FOUND;
}

#define SYS_BO_IDX                0
#define FDD_BO_IDX                1
#define HDD_BO_IDX                2
#define CD_BO_IDX                 3
#define USB_BO_IDX                4
#define NET_BO_IDX                5
#define BEV_BO_IDX                6

typedef struct {
    BBS_TYPE        Type;
    UINT8           *EntryInLegVar;
    UINT16          BoSize;
    UINT16          PathSize;
    UINT16          NoOfDev;
} LEG_BO_PARAM;

EFI_STATUS
BotCreateNew (
  IN OUT OEM_BOOT_ORDER_TABLE     **SmBo,
  IN OUT UINT16                   *SmBoSize
  )
/*++

Routine Description:
 Create Sm data

Arguments:
  **SmBo        - Pointer to SmBo  
  *smBoSize     - Size of SmBo
Returns:
  EFI_SUCCESS if pass,
  EFI Error Status if failed
--*/  
{
  UINT16                    *BootOrder = NULL;
  UINTN                     BootOrderSize = 0;
  UINT8                     *LegDevOrder = NULL;
  UINTN                     LegDevOrderSize = 0;
  UINT8                     *DevOrder = NULL;

  UINTN                     Index;
  UINTN                     LegIndex;
  UINT16                    OptionName[10];

  UINT16                    EfiOrderListSize     =0;
  UINT16                    EfiDevNameSize   =0;
  UINT16                    DevOrderListSize   =0;
  UINT16                    LegacyDevNameSize   =0;

  BBS_TABLE                 *BbsEntry       =NULL;
  UINT8                     BbsDevStr[100];
  UINT8                     *DisabledString  = "<Disabled Device>";
  UINT16                    BbsIndex;
  UINT16                    BbsDevType;

  LEG_BO_PARAM              LegBoParams[8];
  INTN                      Idx;
  UINT8                     BotDevType;
  UINT16                    NoOfDev;
  UINT8                     *TmpBuf;
  UINT8                     *TmpDataBuf;
  
  UINT16                    TmpLen;
  UINT8                     SysBootOrderBuf[20];
  SYSTEM_BOOT_ORDER_DATA    *SysBootOrde = (SYSTEM_BOOT_ORDER_DATA *)SysBootOrderBuf;
  EFI_STATUS                Status;

  EFI_BOOT_MANAGER_LOAD_OPTION  LoadOption;
              
//
// Calculate the BOT size required 
//
  Status = GetVariable2 (
            L"BootOrder",
            &gEfiGlobalVariableGuid,
            &BootOrder,
            &BootOrderSize
            );
  if (EFI_ERROR(Status)) {
    return EFI_OUT_OF_RESOURCES;
  }

  Status = GetVariable2 (
              L"LegacyDevOrder",
             &gEfiLegacyDevOrderVariableGuid,
             &LegDevOrder,
              &LegDevOrderSize
              );
  if (EFI_ERROR(Status)) { 
    // No Legacy/BBS device order
  }

  SetMem (LegBoParams, 8*sizeof(LEG_BO_PARAM),0);

  // Calculate minimum BOT size: header + flag + END of order (0xFF & 0x00) marker
  *SmBo = NULL;
  *SmBoSize = sizeof (OEM_BOOT_ORDER_TABLE) + sizeof (BOT_LEGACY_ORDER_LIST); 

  // Init System BO
  SysBootOrde->OrderType = SYSTEM_BOOT_ORDER_TYPE;
  SysBootOrde->OrderLength = 0;

  // determine device name & path data size
  for (Index = 0; Index < (BootOrderSize / sizeof (UINT16)); Index++) {
    UnicodeSPrint (OptionName, sizeof (OptionName), L"Boot%04x", BootOrder[Index]);
    Status = EfiBootManagerVariableToLoadOption (OptionName, &LoadOption);
    if (EFI_ERROR(Status)) {
      continue;
    }
    
    if (((LoadOption.Attributes & LOAD_OPTION_ACTIVE) == 0) ||
      ((LoadOption.Attributes & LOAD_OPTION_HIDDEN) != 0)) { 
      goto NEXT_DEV;
    }

    // EFI native boot option
    if ((DevicePathType (LoadOption.FilePath) != BBS_DEVICE_PATH) || 
        (DevicePathSubType (LoadOption.FilePath) != BBS_BBS_DP))
    {
      if (0 == EfiOrderListSize) {
        // First active EFI boot option, add EFI order type to System BO
        SysBootOrderBuf[(UINTN)SysBootOrde->OrderLength + sizeof(SYSTEM_BOOT_ORDER_DATA)] = EFI_DEVICE_ORDER_TYPE;
        SysBootOrde->OrderLength++;
      }

      // Increase EFI BO elements
      EfiOrderListSize += sizeof (UINT16);
      
      // Add EFI device path & name data only if one of them is non-empty
      if (0 != GetDevicePathSize(LoadOption.FilePath) || 0 != StrSize(LoadOption.Description)) {
        EfiDevNameSize += sizeof(BOT_EFI_DEVICE);                               // Add Devcode, length feild length
        EfiDevNameSize += (UINT16)StrSize(LoadOption.Description)
                        + (UINT16)GetDevicePathSize(LoadOption.FilePath);       // Add dev.name, path data length 
      }
    } else {// Legacy boot option
      
      if (NULL == BbsTable || NULL == LegDevOrder) {
        goto NEXT_DEV;
      }

      BbsIndex    = *(UINT16*)LoadOption.OptionalData;
      BbsDevType  = BbsTable[BbsIndex].DeviceType;

      // Get the legacy device list for particular type from "LegacyDevOrder"
      if (EFI_SUCCESS != FindThisLegacyDevOrder ((BBS_TYPE)BbsDevType, LegDevOrder, (UINT16)LegDevOrderSize, &DevOrder)) {
        goto NEXT_DEV;
      }

      DevOrderListSize = 0;
      LegacyDevNameSize = 0;
      NoOfDev = 0;
      
      if (DevOrder) {
        // Note: Legacy device order structure is: BBS_TYPE, (UINT16)Length, (UINT16)DevIndex1, (UINT16)DevIndex2..
        // Determine number of devices.
        LEGACY_DEV_ORDER_ENTRY  *ThisDevOrder = (LEGACY_DEV_ORDER_ENTRY *)DevOrder;
        UINTN                   NameSize = 0;
        
        NoOfDev     = (UINT16)(ThisDevOrder->Length / sizeof(UINT16)) -1;
        for (LegIndex = 0; LegIndex < NoOfDev; LegIndex++) {
          BbsIndex    = ThisDevOrder->Data[LegIndex] & 0x00FF;
          BbsEntry    = &BbsTable[BbsIndex];
          NameSize = 0;
          
          // Check for valid (non-disabled & matching entries)
          if (((ThisDevOrder->Data[LegIndex] & 0xFF00) != 0xFF00) && 
               BbsEntry->BootPriority != BBS_IGNORE_ENTRY ) 
          {
            BuildLegacyDevNameA (BbsEntry, BbsIndex, sizeof (BbsDevStr), BbsDevStr);

            DevOrderListSize += sizeof(UINT8);                            // Add to Device order list size
            NameSize = AsciiStrSize(BbsDevStr);
          } else if (((ThisDevOrder->Data[LegIndex] & 0xFF00) == 0xFF00) ||
                      BbsEntry->BootPriority == BBS_IGNORE_ENTRY ) 
          {
            DevOrderListSize += sizeof(UINT8);                            // Add to Device order list size
            NameSize = AsciiStrSize(DisabledString);
          }

          if (NameSize) {
            LegacyDevNameSize += sizeof(BOT_LEGACY_DEVICE) + (UINT16)NameSize; // Add devcode + device name string size
          }
        }

        NoOfDev = DevOrderListSize;      // # of device found
        // Add device order list header size (order type + order length)
        if (DevOrderListSize) {
          DevOrderListSize += sizeof(BOT_LEGACY_ORDER_LIST);
        }
      }

      Idx = -1;
      BotDevType = 0xFF;
      switch (BbsDevType) {
        case BBS_FLOPPY:
          Idx       = FDD_BO_IDX;
          BotDevType  = FDD_ORDER_TYPE;
          break;
    
        case BBS_HARDDISK:
          Idx       = HDD_BO_IDX;
          BotDevType  = LOCAL_HDD_ORDER_TYPE;
          break;

        case BBS_CDROM:
          Idx       = CD_BO_IDX;
          BotDevType  = CD_DVD_ORDER_TYPE;
          break;
    
        case BBS_USB:
          Idx       = USB_BO_IDX;
          BotDevType  = USB_ORDER_TYPE;
          break;

        case BBS_EMBED_NETWORK:
          Idx       = NET_BO_IDX;
          BotDevType  = NETWORK_ORDER_TYPE;
          break;
    
        case BBS_BEV_DEVICE:
          Idx       = BEV_BO_IDX;
          BotDevType  = BEV_ORDER_TYPE;
          break;
    
        default:
          break;
      }
      // Check if valid legacy device type & 
      // if not alreay processed the same device type. In normal cases, you should not hit this more than once.
      if (( -1 != Idx) && (0 == LegBoParams[Idx].BoSize) && (0 != NoOfDev)) {
        LegBoParams[Idx].Type          = BotDevType;
        LegBoParams[Idx].BoSize        = DevOrderListSize;
        LegBoParams[Idx].PathSize      = LegacyDevNameSize;
        LegBoParams[Idx].EntryInLegVar = DevOrder;
        LegBoParams[Idx].NoOfDev       = NoOfDev;

        // If first time processing this type, add to System BO
        SysBootOrderBuf[SysBootOrde->OrderLength + sizeof(SYSTEM_BOOT_ORDER_DATA)] = BotDevType;
        SysBootOrde->OrderLength++;

        // Add legacy device order list + name size to total BOT size
        (*SmBoSize) += DevOrderListSize + LegacyDevNameSize;
      }
    }
  NEXT_DEV: 
    EfiBootManagerFreeLoadOption(&LoadOption);
  }

  // Add device order list header size (order type + order length)
  // Add EFI device order + device Name/Path size
  if (EfiOrderListSize) {
    EfiOrderListSize += sizeof(BOT_EFI_ORDER_LIST);
    (*SmBoSize) += EfiOrderListSize + EfiDevNameSize;
  }

  // Add device order list header size (order type + order length)
  if (SysBootOrde->OrderLength) {
    (*SmBoSize) += SysBootOrde->OrderLength + sizeof(SYSTEM_BOOT_ORDER_DATA);
  }

  *SmBo = AllocateZeroPool (*SmBoSize);
  ASSERT (*SmBo != NULL);

  //
  // Now create the BOT from current system boot order (EFI BootOrder/Bootxxxx & BBS tables)
  //

  // Initialize BOT header
  InitBotHeader (*SmBo);

  //
  // First create System bootorder list
  //
  TmpBuf = (UINT8 *)(*SmBo + 1);
  if (SysBootOrde->OrderLength) {
    TmpLen = SysBootOrde->OrderLength + sizeof(SYSTEM_BOOT_ORDER_DATA);
    CopyMem (TmpBuf, SysBootOrde, TmpLen);
    TmpBuf += TmpLen;
  }

  // Find start location for the Device name/path data
  TmpDataBuf = TmpBuf + EfiOrderListSize + sizeof (BOT_LEGACY_ORDER_LIST); // + END of BO
  for (Idx = FDD_BO_IDX; Idx <= BEV_BO_IDX; Idx++) {
    TmpDataBuf += LegBoParams[Idx].BoSize;
  }

  //
  // Create Legacy Device order list & Legacy device name/path data now
  //
  for (Idx = FDD_BO_IDX; Idx <= BEV_BO_IDX; Idx++) {

    LEGACY_DEV_ORDER_ENTRY  *ThisDevOrder = (LEGACY_DEV_ORDER_ENTRY*)LegBoParams[Idx].EntryInLegVar;
    BOT_LEGACY_ORDER_LIST   *BotLegacyOrderList = (BOT_LEGACY_ORDER_LIST *)TmpBuf;
    
    if (0 == LegBoParams[Idx].BoSize || NULL == LegBoParams[Idx].EntryInLegVar) {
      continue;
    }

    if (BbsTable == NULL) {
        continue;
    }

    BotLegacyOrderList->OrderType = LegBoParams[Idx].Type;
    BotLegacyOrderList->OrderLength = (UINT8)LegBoParams[Idx].NoOfDev;
    TmpBuf += sizeof(BOT_LEGACY_ORDER_LIST);

    // Get the legacy device list for particular type from "LegacyDevOrder"
    NoOfDev = (ThisDevOrder->Length/ sizeof(UINT16)) -1;
    for (Index = 0; Index < NoOfDev; Index++) {

      BbsIndex = ThisDevOrder->Data[Index] & 0x00FF;
      BbsEntry = &BbsTable[BbsIndex];
      TmpLen = 0;
      if (((ThisDevOrder->Data[Index] & 0xFF00) != 0xFF00) &&          // check for non-disabled devices
           (BbsEntry->BootPriority != BBS_IGNORE_ENTRY)) {
        // Use Bbs table index as Device number & add it to Device Order
        *TmpBuf++ = (UINT8)BbsIndex;
        
        // Copy Device name string into BOT's dev path/name data area
        BuildLegacyDevNameA (BbsEntry, BbsIndex, sizeof (BbsDevStr), BbsDevStr);
        TmpLen =(UINT16) AsciiStrSize(BbsDevStr);
      } else if (((ThisDevOrder->Data[Index] & 0xFF00) == 0xFF00) ||
                 BbsEntry->BootPriority == BBS_IGNORE_ENTRY) {

        *TmpBuf++ = (UINT8)BbsIndex;
        TmpLen = (UINT16) AsciiStrSize(DisabledString);
        CopyMem (BbsDevStr, DisabledString, TmpLen);     // Copy Disabled string + null char
      }

      if (TmpLen) {
        // Device name data: Order type (UINT8), device num (UINT16) & name string (ASCII)
        BOT_LEGACY_DEVICE * BotLegacyDev = (BOT_LEGACY_DEVICE *)TmpDataBuf;
        BotLegacyDev->DeviceType  = LegBoParams[Idx].Type;
        BotLegacyDev->DeviceNum   = BbsIndex;
        
        TmpDataBuf += sizeof (BOT_LEGACY_DEVICE);
        CopyMem (TmpDataBuf, BbsDevStr, TmpLen);     // Copy name string + null char
        TmpDataBuf += TmpLen;
      }
    }
  }

  //
  // Create EFI Boot device order list & name/path data now
  //
  if (0 != EfiOrderListSize) {
    // Init EFI boot device order type header
    BOT_EFI_ORDER_LIST * BotEfiOrderList = (BOT_EFI_ORDER_LIST *)TmpBuf;
    BotEfiOrderList->OrderType    = EFI_DEVICE_ORDER_TYPE;
    BotEfiOrderList->OrderLength  = (UINT8)(EfiOrderListSize - sizeof(BOT_EFI_ORDER_LIST)) / sizeof(UINT16);
    TmpBuf += sizeof(BOT_EFI_ORDER_LIST);

    // Init EFI boot device data
    for (Index = 0; Index < (BootOrderSize / sizeof (UINT16)); Index++) {
      UnicodeSPrint (OptionName, sizeof (OptionName), L"Boot%04x", BootOrder[Index]);
      Status = EfiBootManagerVariableToLoadOption (OptionName, &LoadOption);
      if (EFI_ERROR(Status)) {
        continue;
      }
      
      if ((0 != (LoadOption.Attributes &LOAD_OPTION_ACTIVE)) && 
          (0 == (LoadOption.Attributes &LOAD_OPTION_HIDDEN)))
      {
        if ((DevicePathType (LoadOption.FilePath) != BBS_DEVICE_PATH) || 
            (DevicePathSubType (LoadOption.FilePath) != BBS_BBS_DP))
        {
          // Add EFI boot device number to order list
          *(UINT16 *)TmpBuf = BootOrder[Index];
          TmpBuf += sizeof(UINT16);

          //TmpLen = (UINT16)StrSize(LoadOption.Description);
          // Add EFI device path & name data only if one of them is non-empty
          //    Copy device path & name data - Init the device path/name header
          if (0 != GetDevicePathSize(LoadOption.FilePath)|| 0 != StrSize(LoadOption.Description)) 
          {
            BOT_EFI_DEVICE    *BotEfiDev = (BOT_EFI_DEVICE*)TmpDataBuf;
            
            BotEfiDev->DeviceType   = EFI_DEVICE_ORDER_TYPE;
            BotEfiDev->DeviceNum    = BootOrder[Index];
            BotEfiDev->PathLength   = (UINT16)GetDevicePathSize(LoadOption.FilePath);
            TmpDataBuf += sizeof(BOT_EFI_DEVICE);
            
            // Copy device path & name data
            TmpLen = (UINT16)StrSize(LoadOption.Description);
            CopyMem (TmpDataBuf, LoadOption.Description, TmpLen);
            TmpDataBuf += TmpLen;

            TmpLen = (UINT16)GetDevicePathSize(LoadOption.FilePath);
            CopyMem (TmpDataBuf, LoadOption.FilePath, TmpLen);
            TmpDataBuf += TmpLen;
          }
        }
      }
    }
  }

  //
  // Create END of order (0xFF & 0x00) marker
  //
  ((BOT_LEGACY_ORDER_LIST *)TmpBuf)->OrderType = END_OF_BOOT_ORDER_TYPE;
  ((BOT_LEGACY_ORDER_LIST *)TmpBuf)->OrderLength = 0;

  //
  // Update table length & checksum
  //
  (*SmBo)->Header.Length = *SmBoSize;
  BotUpdateChecksum ((UINT8 *)*SmBo, *SmBoSize, &((*SmBo)->Header.Checksum));

  SafeFreePool (BootOrder);
  return EFI_SUCCESS;
}

VOID *
BotFindDevOrder (
  IN UINT8                    OrderType,
  IN OEM_BOOT_ORDER_TABLE     *SmBo,
  IN UINT16                   SmBoSize
  )
/*++

Routine Description:
 Find Device order in SmBo

Arguments:
  OrderType     - Type to be checked
  *SmBo        - Pointer to SmBo  
  smBoSize     - Size of SmBo
Returns:
  None
--*/
{
  UINT8             *Ptr;
  UINT8             *PtrEnd;
  UINT8             DevType;
  UINT8             OrderLength;

  if (NULL == SmBo) {
    return NULL;
  }

  Ptr     = (UINT8 *)SmBo;
  PtrEnd  = Ptr + (SmBoSize -1);
  Ptr     += sizeof (OEM_BOOT_ORDER_TABLE);       // header + flag

  // Find Device name & data begining 
  while ((*Ptr != 0xFF) && (Ptr < PtrEnd)) {      // End of BO
    DevType     = ((BOT_LEGACY_ORDER_LIST *)Ptr)->OrderType;        // order type
    OrderLength = ((BOT_LEGACY_ORDER_LIST *)Ptr)->OrderLength;      // no. of device in the order list

    // Check if order type looking for
    if (DevType == OrderType) {
      return Ptr;
    }

    // Moving to next order type
    if (DevType == EFI_DEVICE_ORDER_TYPE) {
      Ptr += sizeof(BOT_EFI_ORDER_LIST) + OrderLength*sizeof(UINT16);     // EFI device numbers are words 
    } else {
      Ptr += sizeof(BOT_LEGACY_ORDER_LIST) + OrderLength*sizeof(UINT8);   // legacy device numbers are bytes
    }
  }
  return NULL;
}

UINT8 *
BotGetDevOrderHead (
  IN OEM_BOOT_ORDER_TABLE     *SmBo,
  IN UINT16                   SmBoSize
  )
/*++

Routine Description:
 Skip Device order in SmBo

Arguments:
  *SmBo        - Pointer to SmBo  
  smBoSize     - Size of SmBo
Returns:
  Pointer next to the skip order
--*/
{
  UINT8             *Ptr;

  Ptr = (UINT8 *)SmBo;
  Ptr = Ptr + sizeof (OEM_BOOT_ORDER_TABLE); // header + flag
  return Ptr;
}

UINT8 *
BotGetDevNameHead (
  IN OEM_BOOT_ORDER_TABLE     *SmBo,
  IN UINT16                   SmBoSize
  )
/*++

Routine Description:
 Skip Device Name in SmBo

Arguments:
  *SmBo        - Pointer to SmBo  
  smBoSize     - Size of SmBo
Returns:
  Pointer next to the skip order
--*/
{
  UINT8             *Ptr;
  UINT8             *PtrEnd;
  UINT8             DevType;
  UINT8             OrderLength;

  Ptr = (UINT8 *)SmBo;
  PtrEnd = Ptr + (SmBoSize -1);
  Ptr = Ptr + sizeof (OEM_BOOT_ORDER_TABLE); // header + flag

  // Find Device name & data begining 
  while ((*Ptr != 0xFF) && (Ptr < PtrEnd)) {  // End of BO
    DevType = *Ptr++;                         // order type
    OrderLength = *Ptr++;                     // no. of device in the order list
    if (DevType == EFI_DEVICE_ORDER_TYPE) {
      Ptr = Ptr + (OrderLength * 2);          // EFI device numbers are words 
    } else {
      Ptr = Ptr + OrderLength;                // legacy device numbers are bytes
    }
  }
  Ptr++;     // pass end.of order type - 0ffh
  Ptr++;     // pass end.of list - 0 bo len
  return Ptr;
}

UINT8 *
BotGetNextDevOrder (
  IN UINT8                    *Bot,
  IN UINTN                    BotSize,
  IN UINT8                    *PrevDevOrder
  )
{
  UINT8             *Ptr;
  UINT8             *PtrEnd;
  UINT8             DevType;
  UINT8             OrderLength;

  if (NULL == Bot) {
    return NULL;
  }

  Ptr     = (UINT8 *)PrevDevOrder;
  PtrEnd  = Bot + (BotSize -1);

  if ((*Ptr != 0xFF) && (Ptr < PtrEnd)) {       
    DevType     = ((BOT_LEGACY_ORDER_LIST *)Ptr)->OrderType;          
    OrderLength = ((BOT_LEGACY_ORDER_LIST *)Ptr)->OrderLength;        

    if (DevType == EFI_DEVICE_ORDER_TYPE) {
      Ptr += sizeof(BOT_EFI_ORDER_LIST) + OrderLength*sizeof(UINT16);     // EFI dev order
    } else {
      Ptr += sizeof(BOT_LEGACY_ORDER_LIST) + OrderLength*sizeof(UINT8);   // legacy dev
    }
  } else {
    return NULL;
  }
  return Ptr;
}

UINT8 *
BotGetNextDevName (
  IN UINT8                    *Bot,
  IN UINTN                    BotSize,
  IN UINT8                    *PrevDevName
  )
{
  UINT8             *Ptr;
  UINT8             *PtrEnd;

  UINTN             StringLen;
  UINTN             DevPathLen;
  UINT8             DevType;

  if (NULL == Bot) {
    return NULL;
  }

  Ptr     = (UINT8 *)PrevDevName;
  PtrEnd  = Bot + (BotSize -1);

  if ((*Ptr != 0xFF) && (Ptr < PtrEnd)) 
  {
    DevType  = ((BOT_LEGACY_DEVICE *)Ptr)->DeviceType;  
    
    if(DevType == EFI_DEVICE_ORDER_TYPE){
      DevPathLen  = ((BOT_EFI_DEVICE*)Ptr)->PathLength;
      Ptr+= sizeof (BOT_EFI_DEVICE);
      StringLen = (StrLenLocal((CHAR16*)Ptr) + 1) * sizeof(CHAR16);
      Ptr += StringLen + DevPathLen;
    } else {    
      Ptr += sizeof (BOT_LEGACY_DEVICE);
      StringLen = AsciiStrSize(Ptr);
      Ptr += StringLen ;
    }
  } else {
    return NULL;
  }
  
  return Ptr;
}

#ifdef SERIAL_DBG_MSG
VOID
BotDump(
  IN  VOID     *Bot,
  IN  UINTN     BotSize
  )
{
  UINTN         i;
  UINTN         j;
  UINTN         Lines;
  UINT8         *LocalBot;
  UINT8         *PtrDev = NULL;
  UINT8         *Ptr;
  UINT8         *BotEnd;

  ASSERT(Bot != NULL);
  ASSERT(BotSize > 0);

  LocalBot  = (UINT8 *) Bot;
  BotEnd    = LocalBot + BotSize;
  Lines     = ((BotSize %16) == 0) ? (BotSize / 16) : ((BotSize /16) + 1);

  DEBUG((EFI_D_INFO, "-------------------RAW BOT------------------------\n"));
  for (i = 0; i < Lines; i++) {
    for (j = i*16; (j < BotSize) && (j < (i+1)*16); j++) {
      DEBUG((EFI_D_INFO, "%02X ", LocalBot[j]));
    }
    DEBUG((EFI_D_INFO, "\n"));
  }

  DEBUG((EFI_D_INFO, "-------------------BOT Head------------------------\n"));
  PtrDev = BotGetDevOrderHead((OEM_BOOT_ORDER_TABLE *)LocalBot, (UINT16) BotSize);
  for (Ptr = LocalBot; Ptr < PtrDev; Ptr++) {
    DEBUG((EFI_D_INFO, "%02X ", *Ptr));
  }
  DEBUG((EFI_D_INFO, "\n"));
  

  DEBUG((EFI_D_INFO, "-------------------DEV ORDERS------------------------\n"));
  if (PtrDev < BotEnd)
  {
    do {
      UINT8 DevType = ((BOT_LEGACY_ORDER_LIST *)PtrDev)->OrderType;
      UINT8 Length  = ((BOT_LEGACY_ORDER_LIST *)PtrDev)->OrderLength;

      DEBUG((EFI_D_INFO, "%02X : ", DevType));
      if (DevType == EFI_DEVICE_ORDER_TYPE)
      {
        for (i = 0; i < Length; i += 2) {
          DEBUG((EFI_D_INFO, "%04X ", *(UINT16*)(PtrDev + sizeof(BOT_LEGACY_ORDER_LIST) + i)));
        }
      } else
      {
        for (i = 0; i < Length; i++) {
          DEBUG((EFI_D_INFO, "%02X ", *(PtrDev + sizeof(BOT_LEGACY_ORDER_LIST) + i)));
        }
      }
      DEBUG((EFI_D_INFO, "\n"));

      PtrDev = BotGetNextDevOrder (Bot, BotSize, PtrDev);
    } while ((PtrDev != NULL) && (PtrDev < BotEnd));
  }
  

  DEBUG((EFI_D_INFO, "-------------------DEV NAMES------------------------\n"));
  Ptr = BotGetDevNameHead((OEM_BOOT_ORDER_TABLE *)LocalBot, (UINT16) BotSize);
  if (Ptr < BotEnd)
  {
    do {
      UINT8   DevType = ((BOT_LEGACY_DEVICE *)Ptr)->DeviceType;
      UINT16  DevNo   = ((BOT_LEGACY_DEVICE *)Ptr)->DeviceNum;

      
      if (DevType == EFI_DEVICE_ORDER_TYPE)
      {
        CHAR16      *Name = NULL;
        EFI_DEVICE_PATH_PROTOCOL *DevPath = NULL;

        Name = (CHAR16*)(Ptr + sizeof(BOT_EFI_DEVICE));
        DevPath = (EFI_DEVICE_PATH_PROTOCOL*)(Ptr + sizeof(BOT_EFI_DEVICE)+ 
                                            (StrLenLocal(Name) + 1)*sizeof(CHAR16));
        DEBUG((EFI_D_INFO, "[%04X] %02X : ", DevNo, DevType));
        DEBUG((EFI_D_INFO, "%S\n", Name));
        DEBUG((EFI_D_INFO, "          @ %S", ConvertDevicePathToText(DevPath, FALSE, FALSE)));

      } else
      {
        CHAR8       *Name = NULL;
        Name = Ptr + sizeof(BOT_LEGACY_DEVICE);

        DEBUG((EFI_D_INFO, "[B %02X] %02X : ", DevNo, DevType));
        DEBUG((EFI_D_INFO, "%a", Name));
      }
      DEBUG((EFI_D_INFO, "\n"));

      Ptr = BotGetNextDevName (Bot, BotSize, Ptr);
    } while ((Ptr != NULL) && (Ptr < BotEnd));
  }
}
#endif

UINTN
GetDisabledBootDevCount( 
  IN UINT16                 *BootOrderVar, 
  IN UINTN                  BootOrderSize
  )
/*++

Routine Description:
 Get the number of total disabled devices

Arguments:
  *BootOrderVar        - Pointer to SmBo  
  BootOrderSize     - Size of SmBo
Returns:
  the number of the disabled devices
--*/
{
  UINTN                     NoOfDisabledDev;
  UINT8                     *CurBootOpt;
  UINTN                     CurVarSize;
  UINT16                    OptionName[20];
  UINTN                     Index;
  EFI_STATUS                Status;
  UINT32                    Attribute;

  //
  // Check input arguments
  //
  if (BootOrderVar == NULL) {
    return 0;
  }

  NoOfDisabledDev = 0;

  //
  // Check all Boot### variables in BootOrder to see if any are not active.  If so, increment the counter.
  //
  for (Index = 0; Index < (BootOrderSize / sizeof(UINT16)); Index++) 
  {
    UnicodeSPrint (OptionName, sizeof (OptionName), L"Boot%04x", BootOrderVar[Index]);
    Status = GetVariable2 (OptionName, &gEfiGlobalVariableGuid, &CurBootOpt, &CurVarSize);
    if (EFI_ERROR(Status)) {
      return Status;
    }
    Attribute = *(UINT32 *)CurBootOpt;
    if (((Attribute & LOAD_OPTION_ACTIVE) == 0) || ((Attribute & LOAD_OPTION_HIDDEN) != 0)) {
      NoOfDisabledDev += 1;
    }
    SafeFreePool (CurBootOpt);
  }

  return NoOfDisabledDev;
}

EFI_STATUS
AddDisabledBootOptionsToNewBootVar(
  IN     UINT16             *BootOrderVar, 
  IN     UINTN              BootOrderSize,
  IN OUT UINT16             *NewBoVar, 
  IN OUT UINTN              *NewBoVarStartPosition
)
/*++

Routine Description:
 Add disaboled boot option to boot order

Arguments:
  *BootOrderVar        - Pointer to SmBo  
  BootOrderSize     - Size of SmBo
Returns:
  Efi_SUCCESS if pass,
  EFI_ERROR status if failed
--*/
{
  UINT8                     *CurBootOpt;
  UINTN                     CurVarSize;
  UINT16                    OptionName[20];
  UINTN                     Index;
  EFI_STATUS                Status;
  UINT32                    Attribute;

  //
  // Check input arguments
  //
  if ((BootOrderVar == NULL) || (NewBoVar == NULL) || (NewBoVarStartPosition == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  if (*NewBoVarStartPosition >= (BootOrderSize / sizeof(UINT16))) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Check all Boot### variables in BootOrder to see if any are not active.  
  // If so, add them to the end of the NewVoVar array (if the end has not been reached.)
  //
  for (Index = 0; Index < (BootOrderSize / sizeof(UINT16)); Index++) {
    UnicodeSPrint (OptionName, sizeof (OptionName), L"Boot%04x", BootOrderVar[Index]);
    Status = GetVariable2 (OptionName, &gEfiGlobalVariableGuid, &CurBootOpt, &CurVarSize);
    if (EFI_ERROR(Status)) {
      return EFI_NOT_FOUND;
    }
    Attribute = *(UINT32 *)CurBootOpt;
    if (((Attribute & LOAD_OPTION_ACTIVE) == 0) || ((Attribute & LOAD_OPTION_HIDDEN) != 0)) {
      if (*NewBoVarStartPosition >= (BootOrderSize / sizeof(UINT16))) {
        SafeFreePool (CurBootOpt);
        return EFI_BAD_BUFFER_SIZE;
      }

      NewBoVar[*NewBoVarStartPosition] = BootOrderVar[Index];
      *NewBoVarStartPosition += 1;
    }

    SafeFreePool (CurBootOpt);
  }

  if (*NewBoVarStartPosition != (BootOrderSize / sizeof(UINT16))) {
    return EFI_BAD_BUFFER_SIZE;
  } 

  return EFI_SUCCESS;
}

BOOLEAN
IsLegacyDevDisabled(
  IN UINT8                  DeviceNumber, 
  IN UINT16                 *LegDevOrder,
  IN UINT16                 TotalNumberOfDevices 
  )
/*++

Routine Description:
  Check the device is diabled or not

Arguments:
  DeviceNumber      - the device index to be checked
  *LegDevOrder      - Legacy Device order
  TotalNumberOfDevices - Number of device in this group
Returns:
  TRUE if it is disabled,
  FALSE if not
--*/

{
  UINT16                    Index;

  if (NULL == LegDevOrder) {
    return FALSE;
  }

  for (Index = 0; Index < TotalNumberOfDevices; Index++) {
    if ((LegDevOrder[Index] & 0x00FF) == DeviceNumber) {
      if ((LegDevOrder[Index] & 0xFF00) == 0xFF00) {
        return TRUE;
      }
    }
  }
  return FALSE;
}

/**
  Check whether this device path is a internal shell.

  @param  FilePath              File device path.
                                 
  @retval TRUE                  Internal shell.
  @retval FALSE                 Not internal shell.

**/
BOOLEAN
IsInternalShell(
  IN  EFI_DEVICE_PATH_PROTOCOL          *FilePath
  )
{
  EFI_DEVICE_PATH_PROTOCOL        *Node;
  EFI_GUID                        *EfiShellGuid;
  EfiShellGuid = (EFI_GUID*)PcdGetPtr(PcdShellFile);
  for(Node = FilePath; !IsDevicePathEnd(Node); Node = NextDevicePathNode(Node)){
    if( (DevicePathType (Node) == MEDIA_DEVICE_PATH)&&
        (DevicePathSubType (Node) == MEDIA_PIWG_FW_FILE_DP)
     ){
     if(!CompareMem(EfiShellGuid, &(((MEDIA_FW_VOL_FILEPATH_DEVICE_PATH *)Node)->FvFileName),sizeof(EFI_GUID)))
       return TRUE;
    }
  }

  return FALSE;
}


/**
  Check whether LegacyDev BBS is disabled.

  @param  BbsIndex              The BBS index of LegacyDev, which include BBS table index
                                and BBS disabled attribute.

  @retval TRUE                  The device is disabled via setup.
  @retval FALSE                 Not disabled.

**/
BOOLEAN
IsBbsIndexDisabled (
  IN  UINT16                      BbsIndex
  )
{
  if ((BbsIndex & 0xFF00) != 0) {
    return TRUE;
  }
  return FALSE;
}

/**
  Check LegacyDev BBS index, and only return index part, remove the disabled 
  attribute part.

  @param  BbsIndex              The BBS index of LegacyDev, which include BBS table index
                                and BBS disabled attribute.

  @retval UINT16                The BBS table index.

**/
UINT16
OnlyBbsData (
  IN  UINT16                      BbsIndex
  )
{
  return (BbsIndex & 0xFF);
}

#pragma pack(1)
typedef struct {
  UINT16     BbsIndex;
} LEGACY_BOOT_OPTION_BBS_DATA;
#pragma pack()


#define  BOOT_DESCRIPTION_LENGTH  32

/**
  Build Boot#### variable data from a BBS table entry.

  @param  BbsEntry              Device boot entry in BBS table.
  @param  BbsIndex              Boot entry index in BBS table.
  @param  BootOptionSize        Result variable data size.

  @retval Variable              A Boot#### variable data.

**/
UINT8 *
BuildLegacyBootOption(
  IN BBS_TABLE                    *BbsEntry,
  IN UINT16                       BbsIndex,
  OUT UINTN                       *BootOptionSize
  )
{ 
  EFI_DEVICE_PATH_PROTOCOL        *DevicePath;
  CHAR16                          Description[BOOT_DESCRIPTION_LENGTH + 1];
  CHAR8                           HelpString[BOOT_DESCRIPTION_LENGTH + 1];
  UINTN                           StringLen;
  LEGACY_BOOT_OPTION_BBS_DATA     *OptionalData;
  BBS_BBS_DEVICE_PATH             *BbsNode;
  UINTN                           VariableSize;
  UINT8                           *Variable,*Ptr;
  UINT16                          TmpBbsIndex;

  if (BbsEntry == NULL) {
    return NULL;
  }
 
  TmpBbsIndex = OnlyBbsData (BbsIndex);
  BuildLegacyDevNameString (BbsEntry, TmpBbsIndex, sizeof (Description), Description);

  //
  // Create the BBS device path with description string
  //
  UnicodeStrToAsciiStr (Description, HelpString);
  StringLen   = AsciiStrLen (HelpString);
  DevicePath  = AllocatePool (sizeof (BBS_BBS_DEVICE_PATH) + StringLen + END_DEVICE_PATH_LENGTH);
  ASSERT (DevicePath != NULL);
 
  BbsNode = (BBS_BBS_DEVICE_PATH *) DevicePath;
  SetDevicePathNodeLength (BbsNode, sizeof (BBS_BBS_DEVICE_PATH) + StringLen);
  BbsNode->Header.Type    = BBS_DEVICE_PATH;
  BbsNode->Header.SubType = BBS_BBS_DP;
  BbsNode->DeviceType     = BbsEntry->DeviceType;
  CopyMem (&BbsNode->StatusFlag, &BbsEntry->StatusFlags, sizeof (BBS_STATUS_FLAGS));
  CopyMem (BbsNode->String, HelpString, StringLen + 1);

  SetDevicePathEndNode (NextDevicePathNode (BbsNode));

  //
  // Create the OptionalData
  //
  OptionalData = AllocatePool (sizeof (LEGACY_BOOT_OPTION_BBS_DATA));
  ASSERT (OptionalData != NULL);
  OptionalData->BbsIndex = TmpBbsIndex;

  VariableSize = sizeof (UINT32)
               + sizeof (UINT16)
               + StrSize (Description)
               + GetDevicePathSize (DevicePath)
               + sizeof (LEGACY_BOOT_OPTION_BBS_DATA);

  Variable     = AllocateZeroPool (VariableSize);
  ASSERT (Variable != NULL);

  Ptr             = Variable;
  if(IsBbsIndexDisabled(BbsIndex)){
     *(UINT32 *) Ptr = 0;
  }else{
     *(UINT32 *) Ptr = LOAD_OPTION_ACTIVE;
  }
  Ptr            += sizeof (UINT32);
  *(UINT16 *) Ptr = (UINT16) GetDevicePathSize (DevicePath);
  Ptr            += sizeof (UINT16);
  CopyMem (Ptr, Description, StrSize (Description));
  Ptr            += StrSize (Description);
  CopyMem (Ptr, DevicePath, GetDevicePathSize (DevicePath));
  Ptr            += GetDevicePathSize (DevicePath);
  CopyMem (Ptr, OptionalData, sizeof (LEGACY_BOOT_OPTION_BBS_DATA));


  FreePool (DevicePath);
  FreePool (OptionalData);
  
  *BootOptionSize = VariableSize;

  return Variable;
}


EFI_STATUS
UpdateLegacyBootOption (
  IN BBS_TYPE               BbsDevType,
  IN UINT16                 BootNo,
  IN OUT UINT8              *LegDevOrder,
  IN UINT8                  *BotDevOrder
  )
/*++

Routine Description:
  Update Boot Option

Arguments:
  BbsDevType             - Bbs Device Type
  BootNo                 - Boot Index
  *LegDevOrder           - Pointer to LegacyDevOrder
  *SmOrder               - Pointer to SmOder

Returns:
  EFI_STATUS

--*/  
{
 EFI_STATUS                Status;
  UINT8                     *CurBootOpt = NULL;
  UINTN                     CurVarSize;
  UINT16                    OptionName[sizeof("Boot####")];
  BBS_TABLE                 *BbsEntry;
  UINT16                    BbsIndex;
  UINT8                     *Ptr;
  UINT16                    NoOfDev;
  UINT16                    TopDev;
  UINT16                    *OrigDevOrder = NULL;
  UINT16                    TotalNumberOfDevices;
  VOID                      *TmpBootOption = NULL;
  UINTN                     TmpSize;

  Status = EFI_INVALID_PARAMETER;
  if (NULL == LegDevOrder || NULL == BotDevOrder) {
    return Status;
  }

  UnicodeSPrint(OptionName,sizeof(OptionName), L"Boot%04x",BootNo);
  Status = GetVariable2(OptionName, &gEfiGlobalVariableGuid, &CurBootOpt, &CurVarSize);
  if (EFI_ERROR(Status)) 
     return EFI_INVALID_PARAMETER;

  if (!BdsIsLegacyBootOption (OptionName, &BbsEntry, &BbsIndex)) {
    goto ERR_END_UPDATE;
  }

  Ptr = BotDevOrder;
  LegDevOrder += sizeof (BBS_TYPE);
  //
  // Find # of devices & validate
  //
  NoOfDev = *(UINT16 *)LegDevOrder;
  if (NoOfDev <= 2*sizeof(UINT16)) {
    goto ERR_END_UPDATE;
  }
  NoOfDev = (NoOfDev/sizeof(UINT16)) -1;
  if (NoOfDev != (UINT16)((BOT_LEGACY_ORDER_LIST *)Ptr)->OrderLength) {
    goto ERR_END_UPDATE;
  }

  // 
  // Allocate duplicate legacyDevOrder device for comparison to see if a device is disabled
  //
  TotalNumberOfDevices = NoOfDev;

  //  
  // Buffer for new Boot order
  //
  OrigDevOrder = AllocateZeroPool(TotalNumberOfDevices * sizeof(UINT16));
  if (OrigDevOrder == NULL) {
    goto ERR_END_UPDATE;
  }

  Status = EFI_SUCCESS;
  LegDevOrder += sizeof(UINT16);
  CopyMem (OrigDevOrder, LegDevOrder, TotalNumberOfDevices * sizeof(UINT16));

  Ptr += sizeof (BOT_LEGACY_ORDER_LIST);
  TopDev = (UINT16)*Ptr;
  // Copy new legacy device order/number (i.e. BBS index)
  while (NoOfDev--) {
    *(UINT16 *)LegDevOrder = (UINT16)*Ptr;
    if (IsLegacyDevDisabled(*Ptr, OrigDevOrder, TotalNumberOfDevices)) {
      *(UINT16 *)LegDevOrder |= 0xFF00;
    }

    Ptr++;
    LegDevOrder += sizeof (UINT16);
  }

  //
  // If new top device is same device as current top device, then no change in Bootxxxx
  //
  if ((TopDev*0xff) != (BbsIndex&0xff)) {
    //
    // Different device, so update Bootxxxx    
    //
     TmpBootOption = BuildLegacyBootOption(&BbsTable[TopDev&0xff], (TopDev&0xff), &TmpSize);
     TmpSize = sizeof(TmpBootOption);
     if((NULL == TmpBootOption)||(0 == TmpSize)){
         DEBUG ((EFI_D_INFO, "Sbo Build LegacyBootOption Error\n"));
         goto ERR_END_UPDATE;
      }

     Status = gRT->SetVariable (
                    OptionName,
                    &gEfiGlobalVariableGuid,
                    EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                    TmpSize,
                    TmpBootOption
                    );
  }

ERR_END_UPDATE:
  if (CurBootOpt) SafeFreePool (CurBootOpt);
  if (OrigDevOrder) SafeFreePool (OrigDevOrder);

  return Status;
}


EFI_STATUS
UpdateSystemBootOrder (
  IN OEM_BOOT_ORDER_TABLE     *SmBo,
  IN UINT16                   SmBoSize
  )
/*++

Routine Description:
  Update SmBo

Arguments:
  *SmBo           - Pointer to SrcSmBo
  SmBoSize        - Size of the SrcSmBo

Returns:
  EFI_STATUS

--*/
{
  EFI_STATUS                Status;
  EFI_STATUS                Status1;
  UINT16                    *BootNextVar = NULL;
  UINTN                     BootNextSize;
  UINT8                     *TempBoPtr;
  UINT8                     *DevBo;
  UINT16                    *BootOrderVar = NULL;
  UINT16                    *NewBoVar = NULL;
  UINTN                     BootOrderSize;
  UINT8                     *LegDevOrder = NULL;
  UINTN                     LegDevOrderSize;
  UINT8                     *NewLegDevOrder = NULL;
  UINT16                    NoOfDev; 
  SYSTEM_BOOT_ORDER_DATA    *SysBo;
  UINT8                     *SysBoData;
  BOT_EFI_ORDER_LIST       *EfiBo;
  UINT16                    *EfiBoData;
  UINTN                     i;
  BBS_TYPE                  DevType;
  UINT32                    Attribute;
  UINT16                    BbsIndex;
  UINTN                     OptionNumber;
  BOOLEAN                   Found; 
  UINTN                     NoDisabledDev;
  UINTN                     Index;
 
  //
  // This funtions should only update/adjust the boot order, not boot device information,
  // because when this is called boot device are already matched.
  //
  // BOOT_OPTION for legacy bbs device: attrib, pathlen, str, dev path, bbs_table, bbs_index (uin16)
  //
  if (NULL == SmBo || 0 == SmBoSize) {
    return EFI_INVALID_PARAMETER;
  }

  // Get to System BO
  // Find no. of level1 BO from System BO & EFI device Bo
  SysBo = (SYSTEM_BOOT_ORDER_DATA *)BotFindDevOrder (SYSTEM_BOOT_ORDER_TYPE, SmBo, SmBoSize);
  if ((NULL == SysBo) || (0 == SysBo->OrderLength)) {
    return EFI_INVALID_PARAMETER;
  }
  SysBoData = (UINT8 *)(SysBo + 1);

  // Get EFI Device BO
  EfiBo = (BOT_EFI_ORDER_LIST *)BotFindDevOrder (EFI_DEVICE_ORDER_TYPE, SmBo, SmBoSize);
  if (EfiBo != NULL) {
    if (0 == EfiBo->OrderLength)  return EFI_INVALID_PARAMETER;
    EfiBoData = (UINT16 *)(EfiBo + 1);
    NoOfDev =(UINT16)(SysBo->OrderLength) ;
  } else {
    EfiBoData = NULL;
    NoOfDev =(UINT16)(SysBo->OrderLength) ;
  }

  Status = GetVariable2 (
            L"BootOrder",
            &gEfiGlobalVariableGuid,
            &BootOrderVar,
            &BootOrderSize
            );
  if (EFI_ERROR(Status)) {
    return EFI_DEVICE_ERROR;
  }
  // Make sure Current BO & new BO same in size taking into account disabled boot devices
  NoDisabledDev = GetDisabledBootDevCount(BootOrderVar, BootOrderSize);
  if ((NULL != EfiBo) && ((BootOrderSize/sizeof(UINT16)) != (NoOfDev + NoDisabledDev + EfiBo->OrderLength - 1))) {
    Status = EFI_INVALID_PARAMETER;
    goto END_UPDATE;
  }

  NewBoVar = AllocateZeroPool (BootOrderSize);
  if (NULL == NewBoVar) {
    Status = EFI_OUT_OF_RESOURCES;
    goto END_UPDATE;
  }

  // Get Legacy BO
  Status = GetVariable2 (
            L"LegacyDevOrder",
            &gEfiLegacyDevOrderVariableGuid,
            &LegDevOrder,
            &LegDevOrderSize
            );
  if (EFI_ERROR(Status)) {
    // No Legacy devices, only EFI boot devices
  } else {
    NewLegDevOrder = AllocateZeroPool (LegDevOrderSize);
    if (NULL == NewLegDevOrder) {
      Status = EFI_OUT_OF_RESOURCES;
      goto END_UPDATE;
    }
    // Make copy of Legacy device order
    CopyMem (NewLegDevOrder, LegDevOrder, LegDevOrderSize);
  }

  for (i = 0,Index = 0; i < NoOfDev; ) {
    if (SysBoData[i] == EFI_DEVICE_ORDER_TYPE) {
      // Update new EFI boot order, this should come only once
      if ((NULL != EfiBo)&& (0 != EfiBo->OrderLength) && ((Index + EfiBo->OrderLength)*sizeof(UINT16) <= BootOrderSize)) {
          CopyMem (&NewBoVar[Index], EfiBoData, EfiBo->OrderLength*sizeof(UINT16));
          i++;
          Index += EfiBo->OrderLength;
      } else {
          Status = EFI_INVALID_PARAMETER;
          goto END_UPDATE;
      }
    } else {
      switch (SysBoData[i]) {
        case FDD_ORDER_TYPE:
          DevType = BBS_FLOPPY;
          break;
    
        case LOCAL_HDD_ORDER_TYPE:
        case EXT_HDD_ORDER_TYPE:
          DevType = BBS_HARDDISK;
          break;

        case CD_DVD_ORDER_TYPE:
          DevType = BBS_CDROM;
          break;
    
        case USB_ORDER_TYPE:
          DevType = BBS_USB;
          break;

        case NETWORK_ORDER_TYPE:
          DevType = BBS_EMBED_NETWORK;
          break;
    
        case BEV_ORDER_TYPE:
          DevType = BBS_BEV_DEVICE;
          break;
    
        default:
          DevType = 0xFF;
          break;
      }
      if (0xFF != DevType ) {
        TempBoPtr = BotFindDevOrder (SysBoData[i], SmBo, SmBoSize);
        if (EFI_SUCCESS == FindThisLegacyDevOrder (DevType, NewLegDevOrder, (UINT16)LegDevOrderSize, &DevBo)) {
          // For legacy boot options, use the same Bootxxxx but update them with new top priority device
          if (NULL != DevBo && NULL != TempBoPtr) {

            Found = BdsFindLegacyBootOptionByDevType (
                       BootOrderVar,
                       BootOrderSize / sizeof (UINT16),
                       DevType,
                       &Attribute,
                       &BbsIndex,
                       &OptionNumber
                       );

            if (Found == TRUE) {
              UpdateLegacyBootOption (DevType, (UINT16)OptionNumber, DevBo, TempBoPtr);
              NewBoVar[Index] = (UINT16)OptionNumber;
            } else {
              Status = EFI_INVALID_PARAMETER;
              goto END_UPDATE;
            }
          }
        }
      }
      i++;
      Index++;
    }
  }

  if (NoDisabledDev != 0) {
    Status = AddDisabledBootOptionsToNewBootVar(BootOrderVar, BootOrderSize, NewBoVar, &Index);
    if (EFI_ERROR (Status)) {
      goto END_UPDATE;
    }
  }

  Status = EFI_SUCCESS;
  // Write updated Boot order if different
  if (CompareMem(NewBoVar, BootOrderVar, BootOrderSize)) {
    Status = gRT->SetVariable (
                  L"BootOrder", 
                  &gEfiGlobalVariableGuid,
                  VAR_FLAG,
                  BootOrderSize, 
                  NewBoVar
                  );
    Status1 = GetVariable2 (
                  L"BootNext",
                  &gEfiGlobalVariableGuid,
                  &BootNextVar,
                  &BootNextSize
                  ); 
    if(!EFI_ERROR(Status1)){
      //delete bootnext if update boot order
      Status = gRT->SetVariable (
                    L"BootNext",
                    &gEfiGlobalVariableGuid,
                    0,
                    0,
                    NULL
                    );
    }
  }

  // Write update Legacy device order if different
  if (CompareMem(NewLegDevOrder, LegDevOrder, LegDevOrderSize)) {
    Status = gRT->SetVariable (
                  L"LegacyDevOrder", 
                  &gEfiLegacyDevOrderVariableGuid,
                  VAR_FLAG,
                  LegDevOrderSize, 
                  NewLegDevOrder
                  );
  }
  
END_UPDATE:
  if (BootOrderVar) SafeFreePool (BootOrderVar);
  if (NewBoVar) SafeFreePool (NewBoVar);
  if (LegDevOrder) SafeFreePool (LegDevOrder);
  if (NewLegDevOrder) SafeFreePool (NewLegDevOrder);

  return Status;
}

BOOLEAN
BotCompareDevName (
  IN OEM_BOOT_ORDER_TABLE     *SrcSmBo,
  IN UINT16                   SrcSmBoSize,
  IN OEM_BOOT_ORDER_TABLE     *DstSmBo,
  IN UINT16                   DstSmBoSize
  )
/*++

Routine Description:
  Comparie Device in SmBo

Arguments:
  *SrcSmBo           - Pointer to SrcSmBo
  SrcSmBoSize        - Size of the SrcSmBo
  *DstSmBo           - Pointer to DstSmBo
  DstSmBoSize        - Size of the DstSmBo

Returns:
  TURE if they are the same

--*/
{
  UINT8             *PtrSrc;
  UINT8             *PtrDst;
  UINT8             *PtrSrcEnd;
  UINT8             *PtrDstEnd;
  UINT8             *Tempptr;
  UINT8             Srcindex=0;
  UINT8             Dstindex=0;
  UINT8             i,j;
  BOT_DEVICE_DATA  SrcDevNameData[MAX_BOT_DEVICE_DATA_NUM];
  BOT_DEVICE_DATA  DstDevNameData[MAX_BOT_DEVICE_DATA_NUM];
  BOOLEAN           found;
  UINT16            TmpLen;

  if (SrcSmBo == NULL || DstSmBo == NULL || SrcSmBoSize == 0 || DstSmBoSize == 0) {
    return FALSE;
  }

  PtrSrcEnd = ((UINT8 *)SrcSmBo) + (SrcSmBoSize -1);
  PtrDstEnd = ((UINT8 *)DstSmBo) + (DstSmBoSize -1);

  // Find Device name & data begining 
  PtrSrc = BotGetDevNameHead (SrcSmBo, SrcSmBoSize);
  if (PtrSrc >= PtrSrcEnd) {
    return FALSE;
  }

  PtrDst = BotGetDevNameHead (DstSmBo, DstSmBoSize);
  if (PtrDst >= PtrDstEnd) {
    return FALSE;
  }

  // Check if Name & path data size is same
  if ((PtrSrcEnd - PtrSrc + 1) == (PtrDstEnd - PtrDst + 1)) {
    if (!CompareMem (PtrSrc, PtrDst, (PtrSrcEnd-PtrSrc+1))) {
      return TRUE;
    }
  }

  //if the device name data size is same ,try to reorder and compar them
  if ((PtrSrcEnd-PtrSrc+1) != (PtrDstEnd-PtrDst+1)) 
    return FALSE;

  SetMem(SrcDevNameData,sizeof(BOT_DEVICE_DATA)*MAX_BOT_DEVICE_DATA_NUM,0xff);
  SetMem(DstDevNameData,sizeof(BOT_DEVICE_DATA)*MAX_BOT_DEVICE_DATA_NUM,0xff);

  Tempptr = PtrSrc; 
  while(Tempptr <PtrSrcEnd){
    if(Srcindex  >= MAX_BOT_DEVICE_DATA_NUM)
      break;

    SrcDevNameData[Srcindex].DeviceType = ((BOT_LEGACY_DEVICE *)Tempptr)->DeviceType ;
    SrcDevNameData[Srcindex].DeviceNum  = ((BOT_LEGACY_DEVICE *)Tempptr)->DeviceNum;
    SrcDevNameData[Srcindex].DataStart  = (UINT16)(Tempptr - PtrSrc);
    
    if(SrcDevNameData[Srcindex].DeviceType == EFI_DEVICE_ORDER_TYPE){
      TmpLen = ((BOT_EFI_DEVICE*)Tempptr)->PathLength;
      Tempptr+= sizeof( BOT_EFI_DEVICE);
      SrcDevNameData[Srcindex].DataLen = (UINT16)(StrLenLocal((CHAR16*)Tempptr)*sizeof(CHAR16)+TmpLen+sizeof( BOT_EFI_DEVICE));
      Tempptr+= StrLenLocal((CHAR16*)Tempptr)*sizeof(CHAR16)+TmpLen+2;
    } else {    
      Tempptr+= sizeof( BOT_LEGACY_DEVICE);
      SrcDevNameData[Srcindex].DataLen = (UINT16)(AsciiStrLen(Tempptr)+sizeof( BOT_LEGACY_DEVICE));
      Tempptr+= AsciiStrLen(Tempptr)+1;
    }
    Srcindex ++;
  }

  Tempptr = PtrDst; 
  while(Tempptr <PtrDstEnd){
    if(Dstindex  >= MAX_BOT_DEVICE_DATA_NUM)
      break;

    DstDevNameData[Dstindex].DeviceType = ((BOT_LEGACY_DEVICE *)Tempptr)->DeviceType ;
    DstDevNameData[Dstindex].DeviceNum  = ((BOT_LEGACY_DEVICE *)Tempptr)->DeviceNum;
    DstDevNameData[Dstindex].DataStart  = (UINT16)(Tempptr - PtrDst);
    
    if(DstDevNameData[Dstindex].DeviceType == EFI_DEVICE_ORDER_TYPE){
      TmpLen = ((BOT_EFI_DEVICE*)Tempptr)->PathLength;
      Tempptr+= sizeof( BOT_EFI_DEVICE);
      DstDevNameData[Dstindex].DataLen = (UINT16)(StrLenLocal((CHAR16*)Tempptr)*sizeof(CHAR16)+TmpLen+sizeof( BOT_EFI_DEVICE));
      Tempptr+= StrLenLocal((CHAR16*)Tempptr)*sizeof(CHAR16)+TmpLen+2;
    } else {    
      Tempptr+= sizeof( BOT_LEGACY_DEVICE);
      DstDevNameData[Dstindex].DataLen = (UINT16)(AsciiStrLen(Tempptr)+sizeof( BOT_LEGACY_DEVICE));
      Tempptr+= AsciiStrLen(Tempptr)+1;
    }
    Dstindex ++;
  }

  if(Dstindex != Srcindex)
    return FALSE;

  for(i=0 ; i < Srcindex;i++){
    if(SrcDevNameData[i].DeviceType ==0xFF)
      break;

    found = FALSE;    
    for(j=0; j < Dstindex ; j++){
      //find the same device in Dst
      if((DstDevNameData[j].DeviceType == SrcDevNameData[i].DeviceType)&&(DstDevNameData[j].DeviceNum== SrcDevNameData[i].DeviceNum)){
        if (CompareMem (PtrSrc+SrcDevNameData[i].DataStart, PtrDst+DstDevNameData[j].DataStart, SrcDevNameData[i].DataLen))
          return FALSE; //not same device
        else {
          found = TRUE; 
          break;        //search next souce device
        }
      }
    }

    if(!found)          //cannot find matched device in dst array
      return FALSE;
  }   

  if(i == Srcindex)
    return TRUE;        //all the source device name can find same name in dst list

  return FALSE;
}

BOOLEAN
BotCompareDevOrder (
  IN OEM_BOOT_ORDER_TABLE     *SrcSmBo,
  IN UINT16                   SrcSmBoSize,
  IN OEM_BOOT_ORDER_TABLE     *DstSmBo,
  IN UINT16                   DstSmBoSize
  )
/*++

Routine Description:
  Comparie SmBo, the current and old from Bmc

Arguments:
  *SrcSmBo           - Pointer to SrcSmBo
  SrcSmBoSize        - Size of the SrcSmBo
  *DstSmBo           - Pointer to DstSmBo
  DstSmBoSize        - Size of the DstSmBo

Returns:
  TURE if they are the same

--*/
{
  UINT8             *PtrSrc;
  UINT8             *PtrDst;
  UINT8             *Nptro;
  UINT8             *Cptro;
  UINT8             *PtrSrcEnd;
  UINT8             *PtrDstEnd;

  if (SrcSmBo == NULL || DstSmBo == NULL || SrcSmBoSize == 0 || DstSmBoSize == 0) {
    return FALSE;
  }
  if (SrcSmBoSize != DstSmBoSize) {
    return FALSE;
  }

  PtrSrcEnd = ((UINT8 *)SrcSmBo) + (SrcSmBoSize -1);
  PtrDstEnd = ((UINT8 *)DstSmBo) + (DstSmBoSize -1);

  // Find Device name & data begining 
  Nptro = BotGetDevOrderHead (SrcSmBo, SrcSmBoSize);
  PtrSrc = BotGetDevNameHead (SrcSmBo, SrcSmBoSize);
  if (PtrSrc >= PtrSrcEnd || Nptro >= PtrSrcEnd) {
    return FALSE;
  }

  Cptro = BotGetDevOrderHead (DstSmBo, DstSmBoSize);
  PtrDst = BotGetDevNameHead (DstSmBo, DstSmBoSize);
  if (PtrDst >= PtrDstEnd || Cptro >= PtrDstEnd) {
    return FALSE;
  }

  // Check if order data size is same
  if ((PtrSrc-Nptro) == (PtrDst-Cptro)) {
    if (!CompareMem (Nptro, Cptro, (PtrSrc-Nptro))) {
      return TRUE;
    }
  }
  return FALSE;
}

BOOLEAN
BotIsDevNameListEmpty (
  IN OEM_BOOT_ORDER_TABLE     *SrcSmBo,
  IN UINT16                   SrcSmBoSize
  )
/*++

Routine Description:
  Check if device name/path information present or not in a BOT.

Arguments:
  *SrcSmBo           - Pointer to SrcSmBo
  SrcSmBoSize        - Size of the SrcSmBo

Returns:
  TRUE  - Device name/path info NOT present
  FALSE - Device name/path info present

--*/
{
  UINT8             *Ptr;
  UINT8             *PtrEnd;

  if (SrcSmBo == NULL || SrcSmBoSize == 0) {
    return TRUE;
  }

  PtrEnd = ((UINT8 *)SrcSmBo) + (SrcSmBoSize -1);
  Ptr = BotGetDevNameHead (SrcSmBo, SrcSmBoSize);
  if (Ptr >= PtrEnd) {
    return TRUE;
  }

  return FALSE;
}

BOOLEAN
BotMergeSystemOrder (
  IN OEM_BOOT_ORDER_TABLE     *SrcSmBo,
  IN UINT16                   SrcSmBoSize,
  IN OEM_BOOT_ORDER_TABLE     *DstSmBo,
  IN UINT16                   DstSmBoSize
  )
/*++

Routine Description:
  Merge/reorder the System Boot order type (level1) from SrcSmBo into DstSmBo. DstSmBo
  assumed to contain recent avaialble boot device types.

Arguments:
  *SrcSmBo           - Pointer to SrcSmBo
  SrcSmBoSize        - Size of the SrcSmBo
  *DstSmBo           - Pointer to DstSmBo
  DstSmBoSize        - Size of the DstSmBo

Returns:
  TRUE  - System BO in DstSmBo was modified.
  FALSE - System BO in DstSmBo was NOT modified.

--*/
{
  UINT8             *Nptro;
  UINT8             *Cptro;
  UINT8             *PtrSrcEnd;
  UINT8             *PtrDstEnd;
  UINT8             TmpSo[20];
  
  UINTN             SsboLen;
  UINTN             DsboLen;
  UINT8             *Ssbo;
  UINT8             *Dsbo;
  
  UINT8             TmpDev;
  UINTN             Bidx;
  UINTN             i;
  UINTN             j;
  UINTN             k;

  if (SrcSmBo == NULL || DstSmBo == NULL || SrcSmBoSize == 0 || DstSmBoSize == 0) {
    return FALSE;
  }

  // Get to System BO
  PtrSrcEnd = ((UINT8 *)SrcSmBo) + (SrcSmBoSize -1);
  Nptro = BotFindDevOrder (SYSTEM_BOOT_ORDER_TYPE, SrcSmBo, SrcSmBoSize);
  if (Nptro >= PtrSrcEnd || NULL == Nptro) {
    return FALSE;
  }
  SsboLen = (UINTN)((SYSTEM_BOOT_ORDER_DATA *)Nptro)->OrderLength;
  if (0 == SsboLen) {
    return FALSE;
  }
  Ssbo = Nptro + sizeof (SYSTEM_BOOT_ORDER_DATA);

  // Get to System BO
  PtrDstEnd = ((UINT8 *)DstSmBo) + (DstSmBoSize -1);
  Cptro = BotFindDevOrder (SYSTEM_BOOT_ORDER_TYPE, DstSmBo, DstSmBoSize);
  if (Cptro >= PtrDstEnd || NULL == Cptro) {
    return FALSE;
  }
  DsboLen = (UINTN)((SYSTEM_BOOT_ORDER_DATA *)Cptro)->OrderLength;
  if (0 == DsboLen) {
    return FALSE;
  }
  Dsbo = Cptro + sizeof (SYSTEM_BOOT_ORDER_DATA);

  // Init TmpSo to target system BO; TmpSo will have final merged/rearranged
  //  Dsbo from Ssbo
  CopyMem (TmpSo, Dsbo, DsboLen);
  Bidx = 0;
  for (i = 0; i < SsboLen; i++) {                                          
    // For every device in Ssbo, look for matching device in Dsbo
    //  - only search rest of the unmerged array.
    for (j = Bidx; j < DsboLen; j++) {
      // If device matched, move that device up in order (in Dsbo) specified 
      //  by Ssbo and shift down rest of the devices.
      if (Ssbo[i] == TmpSo[j]) {
        TmpDev = TmpSo[j];           // Temporarily save the matched device
        for (k = j; k > Bidx; k--) {
          TmpSo[k] = TmpSo[k-1];    // shift down rest of the devices
        }
        TmpSo[Bidx] = TmpDev;        // Place the matched device in desired order
        Bidx++;
        break;
      }
    }
  }

  // Now check for any changes from the orginal DstSmBo
  if (CompareMem (TmpSo, Dsbo, DsboLen)) {
    // if different, update the adjusted order into DstSmBo and return Adjusted
    CopyMem (Dsbo, TmpSo, DsboLen);
    return TRUE;
  }

  // System BO in DstSmBo was not modified.
  return FALSE;
}

EFI_STATUS
BotValidate (
  IN OEM_BOOT_ORDER_TABLE     *SmBo,
  IN UINT16                   SmBoSize
  )
/*++

Routine Description:
  Valide SmBo

Arguments:
  SmBo           - Pointer to SmBo
  SmBoSize       - Size of the SmBo

Returns:
  EFI_STATUS

--*/
{
  EFI_STATUS                Status           = EFI_SUCCESS;
  UINT16                    *BootOrderVar           = NULL;
  UINTN                     BootOrderSize;
  UINTN                     NoOfDev; 
  SYSTEM_BOOT_ORDER_DATA    *SysBo           = NULL;
  UINT8                     *SysBoData       = NULL;
  BOT_EFI_ORDER_LIST       *EfiBo           = NULL;
  UINT16                    *EfiBoData       = NULL;
  UINTN                     i, j, k;
  UINTN                     SysOptIndex;
  UINTN                     BootOrderIndex;
  UINTN                     EfiBoIndex;
  UINT8                     *CurBootOpt      = NULL;
  UINTN                     CurVarSize;
  UINT16                    OptionName[20];
  UINTN                     NoDevOrderDevices;
  UINT8                     *TempBoPtr       = NULL;
  UINT8                     *DevOrder        = NULL;
  BBS_TABLE                 *BbsEntry        = NULL;
  UINT16                    BbsIndex;

  //
  // Check the checksum
  //
  if (!BotVerifyChecksum ((UINT8 *)SmBo, SmBoSize)) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Check the BootOrder variable and the System Order entries
  //
  Status = GetVariable2 (
            L"BootOrder",
            &gEfiGlobalVariableGuid,
            &BootOrderVar,
            &BootOrderSize
            );
  if (EFI_ERROR(Status)) {
    return EFI_DEVICE_ERROR;
  }

  NoOfDev = BootOrderSize / sizeof(UINT16);

  // Check for duplicate BootOrder entries
  for (i=0; i < NoOfDev - 1; i++) {
    for (j=i+1; j < NoOfDev; j++) {
      if (BootOrderVar[i] == BootOrderVar[j]) {
        Status = EFI_INVALID_PARAMETER;
        goto VERITY_EXIT;
      }
    }
  }

  // Check for a non-zero length System Order
  SysBo = (SYSTEM_BOOT_ORDER_DATA *)BotFindDevOrder (SYSTEM_BOOT_ORDER_TYPE, SmBo, SmBoSize);
  if ((NULL == SysBo) || (0 == SysBo->OrderLength)) {
    Status = EFI_INVALID_PARAMETER;
    goto VERITY_EXIT;
  }
  SysBoData = (UINT8 *)(SysBo + 1);

  // Check for a non-zero length EFI Order if it exists
  EfiBo = (BOT_EFI_ORDER_LIST *)BotFindDevOrder (EFI_DEVICE_ORDER_TYPE, SmBo, SmBoSize);
  if (EfiBo != NULL) {
    EfiBoData = (UINT16 *)(EfiBo + 1);
  }

  //
  // Validate the boot device entries
  //
  for (SysOptIndex=0; SysOptIndex < SysBo->OrderLength; SysOptIndex++) {
    if (SysBoData[SysOptIndex] == EFI_DEVICE_ORDER_TYPE) {

      if ((NULL == EfiBo) || (0 == EfiBo->OrderLength)) {
        Status = EFI_INVALID_PARAMETER;
        goto VERITY_EXIT;
      }

      // Check for the presence of the EFI device in the BootOrder variable
      for (EfiBoIndex=0; EfiBoIndex < EfiBo->OrderLength; EfiBoIndex++) {
        for (BootOrderIndex=0; BootOrderIndex < NoOfDev; BootOrderIndex++) {
          if (EfiBoData[EfiBoIndex] == BootOrderVar[BootOrderIndex]) {
            break;
          }
        }
        if (BootOrderIndex == NoOfDev) {
          // Matching entry was not found, exit with error
          Status = EFI_INVALID_PARAMETER;
          goto VERITY_EXIT;
        }
      }
    } else {

      // Legacy Device: Check for duplicate entries in the device order list
      TempBoPtr = BotFindDevOrder (SysBoData[SysOptIndex], SmBo, SmBoSize);
      if (TempBoPtr != NULL) {
        NoDevOrderDevices = (UINT16)((BOT_LEGACY_ORDER_LIST *)TempBoPtr)->OrderLength;
        DevOrder = (UINT8 *)TempBoPtr + sizeof (BOT_LEGACY_ORDER_LIST);

        for (j=0; j < NoDevOrderDevices - 1; j++) {
          for (k=j+1; k < NoDevOrderDevices; k++) {
            if (DevOrder[j] == DevOrder[k]) {
              Status = EFI_INVALID_PARAMETER;
              goto VERITY_EXIT;
            }
          }
        }

        // Check the device type for the given device number to see if it matches
        // the device type of the System Order entry.
        if (BbsTable != NULL) {

          for (j=0; j < NoDevOrderDevices; j++) {
            BbsEntry = &BbsTable[DevOrder[j]];
            if (BbsEntry->DeviceType == SysBoData[SysOptIndex]) {
              continue;
            } else if ((BbsEntry->DeviceType == BBS_HARDDISK) && (SysBoData[SysOptIndex] == LOCAL_HDD_ORDER_TYPE)) {
              continue;
            } else if ((BbsEntry->DeviceType == BBS_HARDDISK) && (SysBoData[SysOptIndex] == EXT_HDD_ORDER_TYPE)) {
              continue;
            } else {
              Status = EFI_INVALID_PARAMETER;
              goto VERITY_EXIT;
            }
          }
        }
      }
    }
  }

  // Check for matching legacy device type in the BootOrder list and the System Order
  for (BootOrderIndex=0; BootOrderIndex < NoOfDev; BootOrderIndex++) 
  {
    UnicodeSPrint (OptionName, sizeof (OptionName), L"Boot%04x", BootOrderVar[BootOrderIndex]);
    Status = GetVariable2 (
                  OptionName,
                  &gEfiGlobalVariableGuid,
                  &CurBootOpt,
                  &CurVarSize
                  );
    if (EFI_ERROR(Status)) {
      Status = EFI_INVALID_PARAMETER;
      goto VERITY_EXIT;
    }

    // Skip if EFI boot option
    if (!BdsIsLegacyBootOption (OptionName, &BbsEntry, &BbsIndex)) {
      SafeFreePool (CurBootOpt);
      continue;
    }

    // Skip if disabled
    if (((*(UINT32 *)CurBootOpt) & LOAD_OPTION_ACTIVE) == 0) {
      SafeFreePool (CurBootOpt);
      continue;
    }

    // Check each System Order entry type to see if it matches the 
    // type of the BootOrder entry.
    for (SysOptIndex=0; SysOptIndex < SysBo->OrderLength; SysOptIndex++) {
      if (SysBoData[SysOptIndex] == BbsEntry->DeviceType) {
        break;
      }
      if (BbsEntry->DeviceType == BBS_HARDDISK) {
        if (SysBoData[SysOptIndex] == LOCAL_HDD_ORDER_TYPE) {
          break;
        }
        if (SysBoData[SysOptIndex] == EXT_HDD_ORDER_TYPE) {
          break;
        }
      }
    }

    SafeFreePool (CurBootOpt);
    if (SysOptIndex == SysBo->OrderLength) {
      // Matching entry was not found, exit with error
      Status = EFI_INVALID_PARAMETER;
      goto VERITY_EXIT;
    }
  }

  // Check for duplicate entries in the System Order
  for (i=0; i <(UINTN) (SysBo->OrderLength - 1); i++) {
    for (j=i+1; j < SysBo->OrderLength; j++) {
      if (SysBoData[i] == SysBoData[j]) {
        Status = EFI_INVALID_PARAMETER;
        goto VERITY_EXIT;
      }
    }
  }

VERITY_EXIT:
  if (BootOrderVar) SafeFreePool (BootOrderVar);

  return Status;
}


EFI_STATUS
GetSmBoData (
  OUT OEM_BOOT_ORDER_TABLE        **SmBo,
  OUT UINT16                      *SmBoSize,
  OUT OPTIONAL UINT8              *SmFlag
  )
/*++

Routine Description:
  Get BO table from BMC

Arguments:
  SmBo
  SmBoSize
  SmFlag

Returns:
  EFI_INVALID_PARAMETER
  EFI_DEVICE_ERROR
  EFI_SUCCESS
  EFI_OUT_OF_RESOURCES

--*/
{
  OEM_BOOT_ORDER_TABLE        BmcSmBoHdr;
  OEM_BOOT_ORDER_TABLE        *BmcSmBo;
  EFI_STATUS                  Status;
  UINTN                       BmcSmBoSize;

  if ( NULL == SmBo || NULL == SmBoSize) {
    return EFI_INVALID_PARAMETER;
  }

  // Init return params to null
  *SmBo = NULL;
  *SmBoSize = 0;

  // Get BOT header from BMC
  BmcSmBoSize = sizeof(OEM_BOOT_ORDER_TABLE);
  Status = ReadSmOemParameter ((OEM_PARAM_HEADER *)&BmcSmBoHdr, &BmcSmBoSize);
  if ((EFI_BUFFER_TOO_SMALL != Status && EFI_SUCCESS != Status) ||
      (EFI_SMBO_BOOT_ORDER_TABLE_SIGN != BmcSmBoHdr.Header.ParamSign))  {

    return EFI_DEVICE_ERROR;  
  }

  // Update BOT in BMC or SM BO is not in cache avaialble, so update cache and return.
  BmcSmBo = AllocateZeroPool (BmcSmBoSize);
  if (NULL == BmcSmBo) {
    return EFI_OUT_OF_RESOURCES;
  }

  // Read whole BOT
  Status = ReadSmOemParameter ((OEM_PARAM_HEADER *)BmcSmBo, &BmcSmBoSize);
  if (!EFI_ERROR(Status)) {
    if (BotVerifyChecksum ((UINT8 *)BmcSmBo, (UINT16)BmcSmBoSize)) {
      *SmBo = BmcSmBo;
      *SmBoSize = (UINT16)BmcSmBoSize;  
      if (NULL != SmFlag) {
        *SmFlag = (*SmBo)->BootUpdateFlag;
      }
      // Update BOT cache
      Status = gRT->SetVariable (
                  L"SMBootOrder", 
                  &gEfiGenericVariableGuid,  //gEfiGlobalVariableGuid
                  VAR_FLAG,
                  BmcSmBoSize, 
                  BmcSmBo
                  );
    } else {
      Status = EFI_DEVICE_ERROR;
    }
  }
  
  return Status;  
}

EFI_STATUS
SetSmBoData (
  IN OEM_BOOT_ORDER_TABLE     *SmBo,
  IN UINT16                   SmBoSize,
  IN UINT8                    SmFlag
  )
/*++

Routine Description:
  Update BO table in BMC

Arguments:
  SmBo           - Pointer to SmBo
  SmBoSize       - Size of the SmBo
  SmFlag         - Updata Flag of SmBo

Returns:
  EFI_STATUS

--*/
{
  UINTN                     NewBotSize;
  EFI_STATUS                Status;

  SmBo->BootUpdateFlag = SmFlag & (BIOS_UPDATED_BOOT_ORDER | SM_UPDATED_BOOT_ORDER);
  SmBo->Header.Length = SmBoSize; 
  BotUpdateChecksum ((UINT8 *)SmBo, SmBoSize, &SmBo->Header.Checksum);

  // Update cached SM BOT  
  NewBotSize = (UINTN)SmBoSize;
  Status = gRT->SetVariable (
              L"SMBootOrder", 
              &gEfiGenericVariableGuid,  //gEfiGlobalVariableGuid
              VAR_FLAG,
              NewBotSize, 
              SmBo
              );
  // Update BOT in BMC
  Status = WriteSmOemParameter ((OEM_PARAM_HEADER*)SmBo, NewBotSize);

  return Status;
}

BOOLEAN
BotCompareUpdateSystemOrder(
  IN OEM_BOOT_ORDER_TABLE     *SrcSmBo,
  IN UINT16                   SrcSmBoSize,
  IN OEM_BOOT_ORDER_TABLE     *DstSmBo,
  IN UINT16                   DstSmBoSize
  )
/*++

Routine Description:
  Comparie system BOT table only , this function most be triggerred in USB hot plug case.Now we only handle system boot order with hot plug

Arguments:
  *SrcSmBo           - Pointer to SrcSmBo
  SrcSmBoSize        - Size of the SrcSmBo
  *DstSmBo           - Pointer to DstSmBo
  DstSmBoSize        - Size of the DstSmBo

Returns:
  TURE if they are the same

--*/ 
{
  SYSTEM_BOOT_ORDER_DATA    *SrcSysBo;
  UINT8                     *SrcSysBoData;
  SYSTEM_BOOT_ORDER_DATA    *DstSysBo;
  UINT8                     *DstSysBoData;  
  UINT8                     i,j;
  BOOLEAN                   found;

  SrcSysBo = (SYSTEM_BOOT_ORDER_DATA *)BotFindDevOrder (SYSTEM_BOOT_ORDER_TYPE, SrcSmBo, SrcSmBoSize);
  if ((NULL == SrcSysBo) || (0 == SrcSysBo->OrderLength)) {
    return FALSE;
  }
  SrcSysBoData = (UINT8 *)(SrcSysBo + 1);
  
  DstSysBo = (SYSTEM_BOOT_ORDER_DATA *) BotFindDevOrder (SYSTEM_BOOT_ORDER_TYPE, DstSmBo, DstSmBoSize);
  if ((NULL == DstSysBo) || (0 == DstSysBo->OrderLength)) {
    return FALSE;
  }
  DstSysBoData = (UINT8 *)(DstSysBo + 1);
  
  if(SrcSysBo->OrderLength != DstSysBo->OrderLength)
    return FALSE;

  if((DstSmBo->BootUpdateFlag & SM_UPDATED_BOOT_ORDER) == 0)
    return FALSE;

  // if system bot table is same between src and dst and dst requires update boot order table
  for(i=0 ; i < SrcSysBo->OrderLength;i++){
    found = FALSE;
    
    for(j=0; j < DstSysBo->OrderLength ; j++){
      if (*(UINT8*)(SrcSysBoData+i) == *(UINT8*)(DstSysBoData+j)){
        found = TRUE; 
        break;          //search next souce device
      }
    }
    
    if(!found)          //cannot find matched type in dst array
      return FALSE;
  }   

  //all the source type name can find same name in dst list
  //and dst boot order is required by BMC 
  if(i == SrcSysBo->OrderLength){
    CopyMem(SrcSysBoData,DstSysBoData,SrcSysBo->OrderLength);
    return TRUE;
  }

  return FALSE;
}

EFI_STATUS
ProcessIpmiF2BootOrderTable (
  VOID
  )
{
  OEM_BOOT_ORDER_TABLE      *NewBot          = NULL;
  UINT16                    NewBotSize       = 0;
  EFI_STATUS              Status;

  //
  // Create BOT data from current Boot order
  //
  BotCreateNew (&NewBot, &NewBotSize);

  Status = SetSmBoData (NewBot, NewBotSize, BIOS_UPDATED_BOOT_ORDER);

  return Status;
}

BOOLEAN
PlatformUpdateSmBootOrder (
  VOID
  )
/*++

Routine Description:

Arguments:

  None
  
Returns:

  EFI_STATUS if pass

--*/
{
  EFI_STATUS                Status;
  BOOLEAN                   BoFlag;
  
  EFI_LEGACY_BIOS_PROTOCOL  *LegacyBios = NULL;
  UINT16                    HddCount          = 0;
  HDD_INFO                  *LocalHddInfo     = NULL;
  UINT16                    BbsCount          = 0;  
  STATIC BOOLEAN            SmBoDone          = FALSE;

  OEM_BOOT_ORDER_TABLE      *NewBot          = NULL;
  UINT16                    NewBotSize       = 0;
  OEM_BOOT_ORDER_TABLE      *RemoteBot          = NULL;
  UINT16                    RemoteBotSize       = 0;

  if (SmBoDone) {           // Do this only once
    return FALSE;
  }
  SmBoDone = TRUE;
  BoFlag = FALSE;

  Status = gBS->LocateProtocol (&gEfiLegacyBiosProtocolGuid, NULL, &LegacyBios);
  if (!EFI_ERROR (Status)) {
    Status = LegacyBios->GetBbsInfo (LegacyBios, &HddCount, &LocalHddInfo, &BbsCount, &BbsTable);
    if (EFI_ERROR(Status)) {
      DEBUG((EFI_D_ERROR, "[IpmiBoot] LegacyBios->GetBbsInfo failed.\n"));
      //return FALSE;
    }
  }

  //
  // Create BOT data from current Boot order
  //
  BotCreateNew (&NewBot, &NewBotSize);
  DEBUG((EFI_D_INFO, "-------------------NEW BOT------------------------\n"));
#ifdef SERIAL_DBG_MSG
  BotDump (NewBot, NewBotSize);
#endif

  //
  // Get BOT from BMC & check if BOT is present in BMC
  //
  Status = GetSmBoData(&RemoteBot, &RemoteBotSize, NULL);
  DEBUG((EFI_D_INFO, "GetSmBoData = %r\n", Status));
  if (!gLoadDefault && !EFI_ERROR(Status) && RemoteBot != NULL && RemoteBotSize != 0) {

    DEBUG((EFI_D_INFO, "-------------------BMC BOT------------------------\n"));
#ifdef SERIAL_DBG_MSG
    BotDump (RemoteBot, RemoteBotSize);
#endif

    // Check if same devices present
    // Same Boot devices - check for BO change
    if (BotCompareDevName (NewBot, NewBotSize, RemoteBot, RemoteBotSize) == TRUE) {

      DEBUG((EFI_D_INFO, "[IpmiBoot] BotCompareDevName = TRUE\n"));
      if (BotCompareDevOrder (NewBot, NewBotSize, RemoteBot, RemoteBotSize) == TRUE) {
        DEBUG((EFI_D_INFO, "[IpmiBoot] BotCompareDevOrder BOT match, exit!\n"));
        goto BOT_END;
      }
  
      // Boot devices same & only order change, check who updated the order
      if (RemoteBot->BootUpdateFlag & SM_UPDATED_BOOT_ORDER) {
        DEBUG((EFI_D_INFO, "[IpmiBoot] BOT updated, sync to local!\n"));
        Status = BotValidate (RemoteBot, RemoteBotSize);
        if (!EFI_ERROR(Status)) {
          Status = UpdateSystemBootOrder (RemoteBot, RemoteBotSize);  // BO updated from SM or CMM, so adjust system BO
          if (!EFI_ERROR(Status)) {
            Status = SetSmBoData (RemoteBot, RemoteBotSize, 0);       // Clear the flag after update
          }
        }
        BoFlag = TRUE;
        goto BOT_END;
      } 
    } else if(BotCompareUpdateSystemOrder(NewBot, NewBotSize, RemoteBot, RemoteBotSize) == TRUE){

      DEBUG((EFI_D_INFO, "[IpmiBoot] BMC BOT, same system order can be found, but devices not identical.\n"));
      //
      //if arrive here, most cases are hot plug happend, now we only support update group level boot ordre in this case
      //
      if (RemoteBot->BootUpdateFlag & SM_UPDATED_BOOT_ORDER) {

        DEBUG((EFI_D_INFO, "[IpmiBoot] BMC BOT, system order updated, sync to local.\n"));
         /*Status = gRT->SetVariable (
              L"bboBios2", 
              &gEfiGenericVariableGuid,  //gEfiGlobalVariableGuid
              VAR_FLAG,
              NewBotSize, 
              NewBot
              );*/
    
          //system bbo is copied from BMC to Bios
          Status = UpdateSystemBootOrder (NewBot, NewBotSize);
          if (!EFI_ERROR(Status)) {
            //Status = SetSmBoData (RemoteBot, RemoteBotSize, 0);   // Clear the flag after update
            Status = SetSmBoData (NewBot, NewBotSize, 0);
          }
        
        BoFlag = TRUE;
        goto BOT_END;
      } 
      
    }  else {
      DEBUG((EFI_D_INFO, "[IpmiBoot] BMC BOT, both devices and system order not same.\n"));
      if (RemoteBot->BootUpdateFlag & SM_UPDATED_BOOT_ORDER) {
        DEBUG((EFI_D_INFO, "[IpmiBoot] BMC BOT, SM/CMM requested BO change.\n"));
        //
        // Check if Boot device name/path information empty, i.e. booting for first
        //  time with only device type level BO (level1 - system BO) set by SM/CMM.
        //
        if (BotIsDevNameListEmpty (RemoteBot, RemoteBotSize)) {
          DEBUG((EFI_D_INFO, "[IpmiBoot] BMC BOT, Merge system BO only from SM BOT to BIOS BOT.\n"));
          if (BotMergeSystemOrder (RemoteBot, RemoteBotSize, NewBot, NewBotSize)) {
            Status = BotValidate (NewBot, NewBotSize);
            if (!EFI_ERROR(Status)) {
              Status = UpdateSystemBootOrder (NewBot, NewBotSize);
              BoFlag = TRUE;
            }
          }
          // Update SM BOT after with merged SysBO and device order & device name/path
          //  information from BIOS BOT and Clear SM updated flag & Set BIOS updated flag.
          // ** Falls through to main update call **
        }
      }
    }    
  }

// Control comes here because, 
//  * No SM-BOT in BMC, so write BIOS-BOT into BMC -OR-
//  * Boot device order/info is empty & only system BO was present in SM-BOT
//  * Boot devices different (devices removed/added) between BIOS-BOT & SM-BOT -OR-
//  * BO-BOT different due to BO update by user (BIOS setup)
// so create/update BOT in BMC from current BIOS-BOT & set BIOS updated flag
  Status = SetSmBoData (NewBot, NewBotSize, BIOS_UPDATED_BOOT_ORDER);
  
BOT_END:
  return BoFlag;
}

EFI_STATUS
IpmiSmSetBootOptions (
  IN  UINT8                           ParameterType,
  IN  UINT8                           Valid,
  IN  UINT8                           *ParamData,
  IN  UINT8                           *ParamSize
  )
/*++

Routine Description:

Arguments:

  ParameterType
  Parameter data [0] - set selector
  Parameter data [1] - block selector
  Parameter data [2] - parameter data


Returns:

  EFI_STATUS

--*/
{
  EFI_STATUS                Status;
  UINT8                     Buffer[KCS_MAX_SIZE];
  UINT8                     ResponseSize;
  SET_BOOT_OPTIONS_REQUEST  *SetRequest;
  SET_BOOT_OPTIONS_RESPONSE SetResponse;
  UINT8                     RequestSize;

  if (*ParamSize > (KCS_MAX_SIZE-0x10)) {
    return EFI_OUT_OF_RESOURCES;
  }

  gBS->SetMem (Buffer, KCS_MAX_SIZE, 0);
  SetRequest                        = (SET_BOOT_OPTIONS_REQUEST *) &Buffer[0];
  SetRequest->ParameterSelector     = (UINT8) ParameterType;
  SetRequest->MarkParameterInvalid  = (Valid? 0 : 1);

  if (*ParamSize && Valid){
    CopyMem (
      SetRequest->ParameterData,
      ParamData,
      *ParamSize
      );
  }

  ResponseSize = sizeof (SET_BOOT_OPTIONS_RESPONSE);
  RequestSize = *ParamSize + sizeof (SET_BOOT_OPTIONS_REQUEST)-1;
  Status = EfiSendCommandToBMC (
            EFI_SM_NETFN_CHASSIS,
            EFI_CHASSIS_SET_SYSTEM_BOOT_OPTIONS,
            (UINT8 *) SetRequest,
            RequestSize,
            (UINT8 *) &SetResponse,
            &ResponseSize
            );

  //
  // What error to check for
  //
  if (EFI_ERROR (Status)) {
    return Status;
  }

  return EFI_SUCCESS;
}

EFI_STATUS
IpmiSmGetBootOptions (
  IN  UINT8                           ParameterType,
  IN OUT UINT8                        *ParamData,
  IN OUT UINT8                        *ParamSize
  )
/*++

Routine Description:

Arguments:

  ParameterType
  ResponseParameters

Returns:

  EFI_STATUS

--*/
{
  EFI_STATUS                Status;
  UINT8                     Buffer[KCS_MAX_SIZE];
  UINT8                     ResponseSize;
  GET_BOOT_OPTIONS_REQUEST  *GetRequest;
  GET_BOOT_OPTIONS_RESPONSE *GetResponse;

  if (*ParamSize > (KCS_MAX_SIZE-0x10)) {
    return EFI_OUT_OF_RESOURCES;
  }

  gBS->SetMem (Buffer, KCS_MAX_SIZE, 0);
  GetRequest                    = (GET_BOOT_OPTIONS_REQUEST *) &Buffer[0];
  GetRequest->ParameterSelector = (UINT8) ParameterType;
  GetRequest->SetSelector       = ParamData[0];
  GetRequest->BlockSelector     = ParamData[1];

  GetResponse = (GET_BOOT_OPTIONS_RESPONSE *) (Buffer + sizeof (GET_BOOT_OPTIONS_REQUEST));
  ResponseSize = *ParamSize + sizeof (GET_BOOT_OPTIONS_RESPONSE)-1;
  Status = EfiSendCommandToBMC (
            EFI_SM_NETFN_CHASSIS,
            EFI_CHASSIS_GET_SYSTEM_BOOT_OPTIONS,
            (UINT8 *) GetRequest,
            sizeof (GET_BOOT_OPTIONS_REQUEST),
            (UINT8 *) GetResponse,
            &ResponseSize
            );

  //
  // What error to check for
  //
  if (EFI_ERROR (Status)) {
    return Status;
  }

  CopyMem (ParamData, GetResponse->ParameterData, ResponseSize - (sizeof (GET_BOOT_OPTIONS_RESPONSE)-1));
  *ParamSize = ResponseSize - (sizeof (GET_BOOT_OPTIONS_RESPONSE)-1);
  return EFI_SUCCESS;
}
