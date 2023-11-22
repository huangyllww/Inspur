/** @file

Copyright (c) 2006 - 2021, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  RasElogEventLib.c

Abstract:
  Source file for the RasElogEventLib.

Revision History:

**/
#include <PiDxe.h>
#include "RasElogEventLib.h"
#include <IndustryStandard/IpmiNetFnStorage.h>
#include <Ipmi/IpmiNetFnStorageDefinitions.h>
#include <Library/IpmiBaseLib.h>






EFI_STATUS
SendErrorLogDataToBmc (
  IN  UINT8                         *ElogData,
  IN  UINTN                         DataSize
  )
{
  return EFI_UNSUPPORTED;
}






EFI_STATUS
SendElogEventMca (
  IN  UINT8        *ErrorRecord,
  IN  UINT8        BankIndex,
  IN  DIMM_INFO    *DimmInfo
)
{
  return EFI_UNSUPPORTED;
}



EFI_STATUS
SendElogEventNbio (
  IN  UINT8        *ErrorRecord
  )
{
  return EFI_UNSUPPORTED;
}


EFI_STATUS
SendElogEventPcie (
  IN  UINT8        *ErrorRecord,
  IN  UINT32       PcieUncorrStatus,
  IN  UINT32       PcieCorrStatus
  )
{
  return EFI_UNSUPPORTED;
}




EFI_STATUS
SendElogEventMemTraining (
  IN  UINT8        Socket,
  IN  UINT8        Channel,
  IN  UINT8        Dimm
  )
{
  return EFI_UNSUPPORTED;
}


EFI_STATUS
EFIAPI
RasElogEventLibConstructor (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  return EFI_SUCCESS;
}


