/** @file
==========================================================================================
      NOTICE: Copyright (c) 2006 - 2020 Byosoft Corporation. All rights reserved.
              This program and associated documentation (if any) is furnished
              under a license. Except as permitted by such license,no part of this
              program or documentation may be reproduced, stored divulged or used
              in a public system, or transmitted in any form or by any means
              without the express written consent of Byosoft Corporation.
==========================================================================================

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Uefi.h>
#include <PiPei.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/HobLib.h>
#include <Library/IoLib.h>
#include <GnbDxio.h>
#include <AmdPcieComplex.h>
#include <AmdCpmCommon.h>
#include <AmdCpmDefine.h>
#include <Ppi/AmdCpmTablePpi/AmdCpmTablePpi.h>
#include <Token.h>


VOID
EFIAPI
ByoUpdateMaxPayload (
  IN OUT UINT8                 *EngineMaxPayload,
  IN     PCIe_ENGINE_CONFIG    *Engine
  )
{
  
}  

EFI_STATUS
EFIAPI
ByoUpdateDxioData (
  IN EFI_PEI_SERVICES               **PeiServices,
  IN AMD_CPM_DXIO_TOPOLOGY_TABLE    *DxioTopologyTableS0Ptr,
  IN AMD_CPM_DXIO_TOPOLOGY_TABLE    *DxioTopologyTableS1Ptr
  )
{
  return  EFI_SUCCESS;
}


