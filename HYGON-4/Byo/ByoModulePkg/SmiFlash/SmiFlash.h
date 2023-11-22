/*++
Copyright (c) 2011 - 2022 Byosoft Corporation. All rights reserved.

This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

Module Name:

  SmiFlash.h

Abstract:

 This file contains the Includes, Definitions, typedefs,
 Variable and External Declarations, Structure and
 function prototypes needed for the SmiFlash driver
--*/

#ifndef __SMI_FLASH_H__
#define __SMI_FLASH_H__

#include <PiSmm.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PcdLib.h>
#include <Library/CacheMaintenanceLib.h>
#include <Library/BiosIdLib.h>
#include <Library/UefiLib.h>
#include <Library/DxeServicesLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Protocol/SmmSwDispatch2.h>
#include <Protocol/SmmCpu.h>
#include <Protocol/NvMediaAccess.h>
#include <Protocol/Smbios.h>
#include <Protocol/SmmVariable.h>
#include <Guid/SetupItems.h>
#include <IndustryStandard/SmBios.h>
#include <Guid/SmBios.h>
#include <Protocol/ByoSmiFlashProtocol.h>
#include <ByoSmiFlashInfo.h>

#include <SmbiosPnp52.h>
#include <Library/ByoCommLib.h>

#define SUPPORT_FD_AREA_TYPE_LOGO         BIT0
#define SUPPORT_FD_AREA_TYPE_MICROCODE    BIT1
#define SUPPORT_FD_AREA_TYPE_OA3          BIT2
#define SUPPORT_FD_AREA_TYPE_ME           BIT3
#define SUPPORT_FD_AREA_TYPE_IRC          BIT4
#define SUPPORT_FD_AREA_TYPE_EC           BIT5
#define SUPPORT_FD_AREA_TYPE_GBE          BIT6
#define SUPPORT_FD_AREA_TYPE_NIS          BIT7
#define SUPPORT_FD_AREA_TYPE_CAP          BIT8

#ifndef SYNC_NVRAM_AT_SMIFLASH_VERIFY
#define SYNC_NVRAM_AT_SMIFLASH_VERIFY 0
#endif

#pragma pack(1)

typedef struct {
  UINT32        Type;                         // IN, Type of flash area
  UINT32        Offset;                       // OUT, Offset from FD base address or base address for FD_AREA_TYPE_FD
  UINT32        Size;                         // OUT, Byte size of flash area
} FD_AREA_INFO;




typedef enum {
    FD_AREA_TYPE_NULL = 0,
	  FD_AREA_TYPE_FD,
    FD_AREA_TYPE_LOGO,
    FD_AREA_TYPE_VDP,
    FD_AREA_TYPE_NV_STORAGE,
    FD_AREA_TYPE_NV_STORAGE_SPARE,
    FD_AREA_TYPE_MICROCODE,
    FD_AREA_TYPE_FVMAIN,
    FD_AREA_TYPE_SMBIOS,
    FD_AREA_TYPE_RECOVERY2_BACKUP,
    FD_AREA_TYPE_RECOVERY2,
    FD_AREA_TYPE_RAW_DATA,
    FD_AREA_TYPE_RECOVERY,
    FD_AREA_TYPE_LOW_4M,
    FD_AREA_TYPE_SETUP_ITEMS_NAME,
    FD_AREA_TYPE_OA3,
    FD_AREA_TYPE_SETUP_ITEMS_OPTION,
    FD_AREA_TYPE_SETUP_ITEMS_STRING,
    FD_AREA_TYPE_ME,
    FD_AREA_TYPE_IRC,
    FD_AREA_TYPE_EC,
    FD_AREA_TYPE_GBE,
    FD_AREA_TYPE_NIS,
    FD_AREA_TYPE_SETUP_ITEMS_ONEOF_STRING,
} FD_AREA_TYPE;


typedef struct {
    UINT32               BufferSize;
    EFI_PHYSICAL_ADDRESS Buffer;
} ROM_HOLE_PARAMETER;

typedef struct {
  CHAR8  NameString[64];
  UINT64 ModifyValue;
} UPDATE_VALUE_PARAMETER;

typedef struct {
  CHAR16    VariableName[64];
  EFI_GUID  Guid;
  UINT16    Offset;
  UINT16    Length;
  UINT32    ItemNumber;
} CHECK_VALUE_PARAMETER;

#pragma pack()


EFI_STATUS HandleSmbiosDataRequest(UPDATE_SMBIOS_PARAMETER *SmbiosPtr);
EFI_STATUS AllocDataBuffer();
EFI_STATUS HandleIfUnlockRequest(ROM_HOLE_PARAMETER *Param);
EFI_STATUS HandleIfLockRequest();
EFI_STATUS HandleIfResetPasswordRequest();
EFI_STATUS HandleUcodeRequest(ROM_HOLE_PARAMETER *Param);       // wz191108 +
EFI_STATUS HandleIfGetLockStatus(UINTN *LockStatus);

extern NV_MEDIA_ACCESS_PROTOCOL  *mMediaAccess;
extern UINT32 mSetupNameSize;
extern EFI_SMM_VARIABLE_PROTOCOL *mSmmVariable;
extern UINT32 mSetupOptionsSize;
extern CHAR8 *mSetupOptions;
extern OPTIONS_DATA  **mOptionsSplit;
extern UINT32 mSetupItemStringSize;
extern UINT32 mSetupItemOneOfOptionStringSize;
extern UINT32 mSetupItemNumber;
extern SETUP_ITEM_INFO *mSetupItem;
extern UINT32 mSetupVariableNumber;
extern SETUP_VARIABLE_TABLE *mSetupVariableTable;

EFI_STATUS
UniToolSetValueOffset (
  UPDATE_VALUE_PARAMETER *VariableParam
  );

EFI_STATUS
UniToolGetValueOffset (
  UPDATE_VALUE_PARAMETER *VariableParam
  );

VOID
LocatSetupItemsDb (
  );

EFI_STATUS
GetSetupItemsName(
  UINT32 Offset, UINT32 Size, UINT8 *Buffer
  );

EFI_STATUS
GetSetupItemsOption(
  UINT32 Offset, UINT32 Size, UINT8 *Buffer
  );

EFI_STATUS
GetSetupItemsNameString(
  UINT32 Offset, UINT32 Size, UINT8 *Buffer
  );

EFI_STATUS
GetSetupValueGroup (
  UPDATE_VALUE_PARAMETER *VariableParam, UINT32 Offset
  );

EFI_STATUS
SetSetupValueGroup (
  UPDATE_VALUE_PARAMETER *VariableParam, UINT32 Offset, UINT32 Size
  );

EFI_STATUS
GetSetupItemsOneofString(
  UINT32 Offset, UINT32 Size, UINT8 *Buffer
  );

EFI_STATUS
EFIAPI
Lock();

EFI_STATUS
EFIAPI
UnLock();

EFI_STATUS
EFIAPI
GetLockStatus(
  UINTN *LockStatus
  );

EFI_STATUS
ChangeVariableDefault(
  IN OPTIONS_DATA                 *pOptions,
  IN VOID                         *Value,           //only in orderedlist this is decided by pOptions->DataType,others this is uint64
  IN UINT32                       Length
  );

EFI_STATUS
UpdateVariableDefault();

EFI_STATUS
EFIAPI
UpdateMinValue(
  IN CHAR16                       *VariableName,
  IN EFI_GUID                     *VendorGuid,
  IN UINT16                       Offset,
  IN UINT64                       Value
  );

EFI_STATUS
EFIAPI
UpdateMaxValue(
  IN CHAR16                       *VariableName,
  IN EFI_GUID                     *VendorGuid,
  IN UINT16                       Offset,
  IN UINT64                       Value
  );

EFI_STATUS
EFIAPI
UpdateDefaultValue(
  IN CHAR16                       *VariableName,
  IN EFI_GUID                     *VendorGuid,
  IN UINT16                       Offset,
  IN UINT64                       Value
  );

EFI_STATUS
EFIAPI
UpdateAttribute(
  IN CHAR16                       *VariableName,
  IN EFI_GUID                     *VendorGuid,
  IN UINT16                       Offset,
  IN UINT16                       Attribute
  );

EFI_STATUS
EFIAPI
UpdateOptionValue (
  IN CHAR16                       *VariableName,
  IN EFI_GUID                     *VendorGuid,
  IN UINT16                       Offset,
  IN UINTN                        Count,
  IN UINT64                       *OptionValueArray
  );

EFI_STATUS
EFIAPI
CheckVariableValid(
  CHECK_VALUE_PARAMETER  *VariableParameter
  );

#endif
