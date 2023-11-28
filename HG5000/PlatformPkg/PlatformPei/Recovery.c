/** @file

Copyright (c) 2006 - 2011, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

Module Name:

  Recovery.c

Abstract:

  PEIM to provide the platform recovery functionality.

--*/


#include "PlatformPei.h"
#include <Ppi/AtaController.h>
#include <Ppi/BootInRecoveryMode.h>
#include <IndustryStandard/Pci.h>


//
// Required Service
//
EFI_STATUS
EnableAtaChannel (
  IN EFI_PEI_SERVICES       **PeiServices,
  IN PEI_ATA_CONTROLLER_PPI *This,
  IN UINT8                  ChannelIndex
  );

UINT32
GetIdeRegsBaseAddr (
  IN  EFI_PEI_SERVICES       **PeiServices,
  IN  PEI_ATA_CONTROLLER_PPI *This,
  OUT IDE_REGS_BASE_ADDR     *IdeRegsBaseAddr
  );

//
// Module globals
//
static PEI_ATA_CONTROLLER_PPI mAtaControllerPpi = {
  EnableAtaChannel,
  GetIdeRegsBaseAddr
};

static EFI_PEI_PPI_DESCRIPTOR mRecoveryPpiList[] = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gEfiPeiBootInRecoveryModePpiGuid,
  NULL
};
  
static EFI_PEI_PPI_DESCRIPTOR mAtaControllerPpiList[] = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gPeiAtaControllerPpiGuid,
  &mAtaControllerPpi
};


EFI_STATUS
EnableAtaChannel (
  IN EFI_PEI_SERVICES               **PeiServices,
  IN PEI_ATA_CONTROLLER_PPI         *This,
  IN UINT8                          ChannelMask
  )
{
  DEBUG((EFI_D_INFO, "%a(M:%X)\n", __FUNCTION__, ChannelMask));

  return EFI_SUCCESS;
}

typedef struct {
  UINTN               Channels;
  IDE_REGS_BASE_ADDR  IoBase[1];
} IDE_IO_BASE_INFO;

UINT32
GetIdeRegsBaseAddr (
  IN  EFI_PEI_SERVICES       **PeiServices,
  IN  PEI_ATA_CONTROLLER_PPI *This,
  OUT IDE_REGS_BASE_ADDR     *IdeRegsBaseAddr
  )
{

  DEBUG((EFI_D_INFO, "IDEChannels\n"));

  return 0;

}


EFI_STATUS
EFIAPI
PeimInitializeRecovery (
  IN EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS  Status;

  Status = (*PeiServices)->InstallPpi(PeiServices, &mRecoveryPpiList[0]);
  ASSERT_EFI_ERROR (Status);

//  Status = (*PeiServices)->InstallPpi(PeiServices, &mAtaControllerPpiList[0]);
  ASSERT_EFI_ERROR (Status);
  
  return Status;
}


