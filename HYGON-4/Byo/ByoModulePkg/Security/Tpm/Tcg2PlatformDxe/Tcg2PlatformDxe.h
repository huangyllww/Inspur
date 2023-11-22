/** @file
  Platform specific TPM2 component for configuring the Platform Hierarchy.

  Copyright (c) 2017 - 2022, Byosoft Corporation. All rights reserved.<BR>
  This software and associated documentation (if any)
  is furnished under a license and may only be used or copied in
  accordance with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be reproduced,
  stored in a retrieval system, or transmitted in any form or by any
  means without the express written consent of Byosoft Corporation.

**/

#ifndef _TCG2_PLATFORM_DXE_H_
#define _TCG2_PLATFORM_DXE_H_

#include <PiDxe.h>

#include <Uefi/UefiBaseType.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PcdLib.h>
#include <Library/PrintLib.h>
#include <Library/Tpm2CommandLib.h>

#include <Library/UefiLib.h>
#include <Library/TpmPlatformHierarchyLib.h>
#include <Protocol/DxeSmmReadyToLock.h>
///
/// Driver Consumed Protocol Prototypes
///
#include <Protocol/Smbios.h>

#define SMBIOS_TYPE43_NUMBER_OF_STRINGS  1
#define NO_STRING_AVAILABLE              0
#define STRING_1                         1

///
/// Non-static SMBIOS table data to be filled later with a dynamically generated value
///
#define TO_BE_FILLED                     0
#define TO_BE_FILLED_STRING              " "        ///< Initial value should not be NULL

#define ASCII_NULL_CHAR                  0x00
#define ASCII_SPACE_CHAR                 0x20

#pragma pack(1)
typedef struct {
  CHAR8 *Description;
} SMBIOS_TYPE43_STRING_ARRAY;
#pragma pack()

#endif // _TCG2_PLATFORM_DXE_H_