/** @file

Copyright (c) 2006 - 2019, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  HddBindDxe.h

Abstract:
  Binding the harddisk(SATA or NVME) according to the configuration set by users.
When this function has been opened, it can't boot from the unbound harddisk.

Revision History:

**/

#ifndef __HDD_BIND_DXE_H__
#define __HDD_BIND_DXE_H__

#include "HddBindDxeVfr.h"
#include <IndustryStandard/Atapi.h>
#include <Guid/MdeModuleHii.h>

#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/DebugLib.h>
#include <Library/HiiLib.h>
#include <Library/DevicePathLib.h>
#include <Library/UefiHiiServicesLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PrintLib.h>
#include <Library/UefiBootManagerLib.h>
#ifndef BYO_ONLY_UEFI
#include <Library/LegacyBootOptionalDataLib.h>
#endif
#include <Protocol/HiiConfigAccess.h>
#include <Protocol/DevicePath.h>
#include <Protocol/ByoPlatformSetupProtocol.h>
#include <Protocol/NvmExpressPassthru.h>
#include <Protocol/DiskInfo.h>
#include <Protocol/ByoDiskInfoProtocol.h>
#include <Protocol/PciIo.h>
#include <Protocol/ReportStatusCodeHandler.h>
#include <Protocol/UefiBbsRelationInfo.h>

#include <SysMiscCfg.h>

#define DESCRIPTION_STRING_MAX_LENGTH      256
#define FIRST_USB_BBS_INDEX                0x30

#define MAXSIZE_OF_MN 41

typedef struct {
  UINT16                     Port;
  EFI_DEVICE_PATH_PROTOCOL   *DevicePath;
  UINT16                     BbsIndex;
  UINTN                     Bus;
  UINTN                     Device;
  UINTN                     Function;
  CHAR16                     Mn[MAXSIZE_OF_MN];
  CHAR8                      Sn[MAXSIZE_OF_SN];
  BOOLEAN                    Selected;
} EXTEND_HDD_DATA;

typedef struct {
  CHAR16                    *Description;
  UINT16                    HddIndex;
  CHAR16                    Mn[MAXSIZE_OF_MN];
  CHAR8                     Sn[MAXSIZE_OF_SN];
  VOID                      *ExtendData;
  UINTN                     ExtendDataSize;
} HDD_BIND_INFO;
       
#define HDD_BIND_PRIVATE_SIGNATURE SIGNATURE_32 ('H', 'D', 'B', 'D')

typedef struct {
  UINTN                            Signature;
  EFI_HII_HANDLE                   HiiHandle;
  EFI_HANDLE                       DriverHandle;
  EFI_HII_CONFIG_ACCESS_PROTOCOL   ConfigAccess;
  HDD_BIND_VAR_DATA                VarData;
  HDD_BIND_INFO                    *HddBindInfo;
  EXTEND_HDD_DATA                  *AllHddInfo;
  UINT16                           HddCount;
  UINT16                           AllHddCount;
  VOID                             *BoundHddNvData;
  UINTN                            BoundHddNvSize;
  BOOLEAN                          UserChoice;
  CHAR16                           *TargetMn;
  CHAR8                            *TargetSn;
  CHAR16                           *Description;
} HDD_BIND_PRIVATE_DATA;

#define HDD_BIND_DATA_FROM_THIS(a)  CR(a, HDD_BIND_PRIVATE_DATA, ConfigAccess, HDD_BIND_PRIVATE_SIGNATURE)

#define HDD_BIND_NV_NAME L"HddSn"
#define HDD_BIND_NV_GUID \
{ 0xbffd15ff, 0x4e3e, 0x44d1, { 0xb3, 0xd7, 0xc7, 0x7b, 0x31, 0x25, 0x7a, 0xa4 } }

EFI_STATUS
EFIAPI
CheckBindHdd (
  EFI_DEVICE_PATH_PROTOCOL                   *Node,
  EFI_DEVICE_PATH_PROTOCOL                   *FilePath
);

VOID
EFIAPI
ShowWarningMsg (
  VOID
  );

EFI_STATUS
EFIAPI
GetHddSnAndMn (
  IN EFI_DEVICE_PATH_PROTOCOL                *Node,
  IN EFI_DEVICE_PATH_PROTOCOL                *DevicePath,
  IN OUT CHAR16                              *Mn,
  IN OUT CHAR8                               *Sn
  );

/**
  Get informations of all harddisks(ATA&&NVME) on system.
  
  @param AllHddInfo return the informations of hard disks.And AllHddInfo need to free by caller
  when AllHddInfo is not NULL.
  @return  The number of hard disks enumerated.
**/
UINT16
EFIAPI
EnumerateAllHddInfo (
  IN OUT EXTEND_HDD_DATA          **AllHddInfo
  );

EFI_DEVICE_PATH_PROTOCOL *
CheckHdShortDp (
  IN  EFI_DEVICE_PATH_PROTOCOL        *DevicePath
  );

/**
  Filter out valid hard disks.
  Feature:
  Valid hard disks:Can match the boot options in hard disk Type.And they will be listed in
  configuration page for users to choose which hard disk to bind.The matching method here
  is to compare the MN(Manufacturer Number) of the hard disk with the MN contained in
  Description of the boot option.

  @param ValidHddInfo return informations of valid hard disks.This parameter is allocated 
  and freed by caller.
  @return  The number of valid hard disks enumerated.
**/
UINT16
EFIAPI
FilterOutValidHdd (
  IN CONST EXTEND_HDD_DATA         *AllHddInfo,
  IN UINT16                        AllHddCount,
  IN UINTN                         CountMax,
  IN OUT HDD_BIND_INFO             *ValidHddInfo
  );

VOID
EFIAPI
GetHddBindNvData (
  VOID
  );

VOID
EFIAPI
UnregisterAtExitBootServices (
  IN EFI_EVENT                               Event,
  IN VOID                                    *Context
  );

EFI_STATUS
EFIAPI
HandleStatusCode (
  IN EFI_STATUS_CODE_TYPE                    CodeType,
  IN EFI_STATUS_CODE_VALUE                   Value,
  IN UINT32                                  Instance,
  IN EFI_GUID                                *CallerId,
  IN EFI_STATUS_CODE_DATA                    *Data
  );

VOID
LegacyCheckBoundHdd (
  VOID                            *Param
);
#endif