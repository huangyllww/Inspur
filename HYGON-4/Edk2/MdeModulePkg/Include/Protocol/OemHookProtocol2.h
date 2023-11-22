/** @file

Copyright (c) 2006 - 2019, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:

Abstract:

Revision History:

$END----------------------------------------------------------------------------
**/

#ifndef __OEM_HOOK_PROTOCOL2_H__
#define __OEM_HOOK_PROTOCOL2_H__

#include <Protocol/AtaPassThru.h>
#include <Protocol/PciIo.h>

// -----------------------------------------------------------------------------
// AtaPassThruHook
//   Invoked after AtaPassThru Protocol installed
// -----------------------------------------------------------------------------
extern EFI_GUID  gAtaPassThruHookProtocolGuid;

#define ATA_PASSTHRU_HOOK_PARAMETER_SIGN  SIGNATURE_32('A', 'P', 'T', 'H')

#define ATA_PT_SATA_MODE_IDE      0
#define ATA_PT_SATA_MODE_AHCI     1
#define ATA_PT_SATA_MODE_RAID     2
#define ATA_PT_SATA_MODE_UNKNOWN  0xFF

typedef struct {
  UINT32                        Sign;
  EFI_HANDLE                    Controller;
  EFI_ATA_PASS_THRU_PROTOCOL    *AtaPassThru;
  EFI_PCI_IO_PROTOCOL           *PciIo;
  UINTN                         SataMode;
} ATA_PASSTHRU_HOOK_PARAMETER;

typedef
VOID
(EFIAPI *EFI_MY_HOOK_PROTOCOL2)(
  VOID  *Param
  );

#endif
