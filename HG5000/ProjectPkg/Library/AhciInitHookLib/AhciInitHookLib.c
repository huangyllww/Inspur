/** @file

Copyright (c) 2006 - 2019, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

By: 
  winddy_zhang

File Name:

Abstract:

Revision History:

$END----------------------------------------------------------------------------
**/

#include <Uefi.h>
#include <Library/AhciInitHookLib.h>
#include <Library/DebugLib.h>
#include <Library/TimerLib.h>
#include <Protocol/PciIo.h>
#include <Library/IoLib.h>
#include <PlatformDefinition.h>


VOID
EFIAPI
AhciOrReg (
  IN EFI_PCI_IO_PROTOCOL  *PciIo,
  IN UINT32               Offset,
  IN UINT32               OrData
  );

UINT32
EFIAPI
AhciReadReg (
  IN EFI_PCI_IO_PROTOCOL  *PciIo,
  IN  UINT32              Offset
  );

VOID
EFIAPI
LibAhciInitHookAfterReset (
    EFI_HANDLE                       Host,
    EFI_PCI_IO_PROTOCOL              *PciIo
  )
{
  UINT32 PciId;
//UINTN  Base, S, B, D, F;


  DEBUG((EFI_D_INFO, "%a\n", __FUNCTION__));

	PciIo->Pci.Read (
             PciIo,
             EfiPciIoWidthUint32,
             0,
             1,
             &PciId
             );
  if(PciId == 0x06221B21){

//MMIO offset ¨C 0D0Ah 
// 1  RW  SATA Port 1 spinup
// 0  RW  SATA Port 0 spinup 

    AhciOrReg(PciIo, 0xD08, 0x30000);
    DEBUG((EFI_D_INFO, "[D08]:%X\n", AhciReadReg(PciIo, 0xD08)));

#if 0
    PciIo->GetLocation (
             PciIo,
             &S,
             &B,
             &D,
             &F
             );

    Base = PCI_DEV_MMBASE(B, D, F);
    MmioOr8(Base + 0xD0A, 3);
    DEBUG((EFI_D_INFO, "(%X,%X,%X)[D0A]:%X\n", B, D, F, MmioRead8(Base + 0xD0A)));
#endif 

  } else {
    DEBUG((EFI_D_INFO, "%X no need patch.\n", PciId));
  }


}



EFI_STATUS
EFIAPI
AhciInitHookLibConstructor (
    VOID
  )
{
  gAhciInitHookAfterReset = LibAhciInitHookAfterReset;
  return EFI_SUCCESS;
}



