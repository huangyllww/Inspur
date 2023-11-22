/** @file
  Instant Debugger PEIM.

Copyright (c) 2021, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

**/

#include <PiPei.h>

#include <Library/DebugLib.h>
#include <Library/DebugAgentLib.h>

/**
  The Entry Point for Debug Agent PEI driver.

  It will invoke Debug Agent Library to enable source debugging feature in PEI phase.

  This function is the Entry point of the CPU I/O PEIM which installs CpuIoPpi.

  @param[in]  FileHandle   Pointer to image file handle.
  @param[in]  PeiServices  Pointer to PEI Services Table

  @retval EFI_SUCCESS    Debug Agent successfully initialized.
  @retval other          Some error occurs when initialized Debug Agent.

**/
EFI_STATUS
EFIAPI
InstantDebuggerPeiInitialize (
  IN EFI_PEI_FILE_HANDLE     FileHandle,
  IN CONST EFI_PEI_SERVICES  **PeiServices
  )
{
  EFI_STATUS                 Status;
  InitializeDebugAgent (DEBUG_AGENT_INIT_PEI, &Status, NULL);
  return Status;
}
