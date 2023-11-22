/** @file

Copyright (c) 2006 - 2020, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  TcmAcpi.h

Abstract:
  Acpi part of TCM Module.

Revision History:

**/



#ifndef  __TCM_DXE_H__
#define  __TCM_DXE_H__
//--------------------------------------------------------------
#include <IndustryStandard/Acpi.h>
#include <Guid/GlobalVariable.h>
#include <Guid/EventGroup.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/DevicePathLib.h>
#include <Library/PrintLib.h>
#include <Library/HobLib.h>
#include <Protocol/TcmService.h>
#include <Protocol/DevicePath.h>
#include <Protocol/AcpiTable.h>
#include <Library/DxeServicesLib.h>
#include <Library/PcdLib.h>

#define EFI_TCM_LOG_AREA_SIZE   0x10000

#pragma pack(1)

typedef struct {
  EFI_ACPI_DESCRIPTION_HEADER       Header;
  UINT16                            PlatformClass;
  UINT32                            Laml;             // Log Area Minimum Length (LAML), 64KB
  EFI_PHYSICAL_ADDRESS              Lasa;             // Log Area Start Address (LASA)
} EFI_TCM_ACPI_TABLE;
#pragma pack()

//--------------------------------------------------------------
#endif

