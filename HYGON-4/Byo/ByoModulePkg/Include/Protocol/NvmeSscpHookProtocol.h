/** @file

Copyright (c) 2022, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

**/

#ifndef __NVME_SSCP_HOOK_PROTOCOL_H__
#define __NVME_SSCP_HOOK_PROTOCOL_H__

#include <Protocol/StorageSecurityCommand.h>
#include <Protocol/NvmExpressPassthru.h>
#include <Protocol/PciIo.h>
#include <Protocol/BlockIo.h>



#define NVME_SSCP_HOOK_PARAMETER_SIGN     SIGNATURE_32('S', 'S', 'C', 'P')

typedef struct {
  UINT32                                 Sign;
  EFI_HANDLE                             Controller;
  EFI_STORAGE_SECURITY_COMMAND_PROTOCOL  *Sscp;
  EFI_NVM_EXPRESS_PASS_THRU_PROTOCOL     *Passthru;
  EFI_PCI_IO_PROTOCOL                    *PciIo;
  EFI_DEVICE_PATH_PROTOCOL               *Dp;
  EFI_BLOCK_IO_PROTOCOL                  *BlockIo;
  UINT32                                 NamespaceId;
  UINT8                                  Sn[21];
  UINT8                                  Mn[41];
} NVME_SSCP_HOOK_PARAMETER;

extern EFI_GUID gNvmeSscpHookProtocolGuid;

#endif

