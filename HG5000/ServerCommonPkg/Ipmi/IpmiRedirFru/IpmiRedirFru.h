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

#ifndef _EFI_IPMI_REDIR_FRU_H_
#define _EFI_IPMI_REDIR_FRU_H_

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>

#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>

#include "EfiServerManagement.h"
#include <Library/IpmiBaseLib.h>

#include <Protocol/RedirFru.h>
#include <Protocol/HiiDatabase.h>
#include <Protocol/HiiString.h>
#include <IndustryStandard/SmBios.h>
#include <Protocol/Smbios.h>

#define MAX_FRU_SLOT            20

#define IPMI_READ_FRU_COMMAND   0x11
#define IPMI_WRITE_FRU_COMMAND  0x12

#define CHASSIS_TYPE_LENGTH     1
#define CHASSIS_TYPE_OFFSET     2
#define CHASSIS_PART_NUMBER     3

#define PRODUCT_MFG_OFFSET      3
#define BOARD_MFG_OFFSET        6
#define CMOS_WAK_TYPE_STORE     0x41

#define SMBIOSTYPE0 0
#define SMBIOSTYPE1 1
#define SMBIOSTYPE2 2
#define SMBIOSTYPE3 3

#define OFFSET0 0
#define OFFSET1 1
#define OFFSET2 2
#define OFFSET3 3
#define OFFSET4 4
#define OFFSET5 5
#define OFFSET6 6
#define OFFSET7 7
#define OFFSET8 8
#define OFFSET9 9

#define STRING1 1
#define STRING2 2
#define STRING3 3
#define STRING4 4
#define STRING5 5
#define STRING6 6
#define STRING7 7
#define STRING8 8
#define STRING9 9

#define  MANAGEMENT_ACCESS_RECORD       0x03
#define  SYSTEM_UUID_SUB_RECORD_TYPE    0x07
#define  RECORD_CHECKSUM_SIZE           sizeof(UINT8)


typedef struct {
  UINTN                     Signature;
  UINT8                     MaxFruSlots;
  UINT8                     NumSlots;
  EFI_FRU_DEVICE_INFO       FruDeviceInfo[MAX_FRU_SLOT];
  EFI_SM_FRU_REDIR_PROTOCOL IpmiRedirFruProtocol;
} EFI_IPMI_FRU_GLOBAL;


EFI_STATUS
EfiGetFruRedirData (
  IN EFI_SM_FRU_REDIR_PROTOCOL            *This,
  IN UINTN                                FruSlotNumber,
  IN UINTN                                FruDataOffset,
  IN UINTN                                FruDataSize,
  IN UINT8                                *FruData
  )
/*++

Routine Description:

  Get Fru Redir Data 

Arguments:

  This
  FruSlotNumber
  FruDataOffset
  FruDataSize
  FruData

Returns:

  EFI_STATUS

--*/
;

VOID
GenerateFruSmbiosData (
  IN EFI_SM_FRU_REDIR_PROTOCOL            *This
  )
/*++

Routine Description:

  Generate Fru Smbios Data

Arguments:

  This

Returns:

  None

--*/
;

#define INSTANCE_FROM_EFI_SM_IPMI_FRU_THIS(a) \
  CR (a, \
      EFI_IPMI_FRU_GLOBAL, \
      IpmiRedirFruProtocol, \
      EFI_SM_FRU_REDIR_SIGNATURE \
      )


#endif
