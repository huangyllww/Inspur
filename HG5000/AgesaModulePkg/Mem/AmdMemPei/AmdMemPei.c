/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Memory API for CZ DDR4, and related functions.
 *
 * Contains code that initializes memory
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  PSP
 * @e \$Revision: 309090 $   @e \$Date: 2014-12-09 12:28:05 -0600 (Tue, 09 Dec 2014) $
 *
 */
/*****************************************************************************
 *
 * 
 * Copyright 2016 - 2019 CHENGDU HAIGUANG IC DESIGN CO., LTD. All Rights Reserved.
 * 
 * HYGON is granting you permission to use this software (the Materials)
 * pursuant to the terms and conditions of your Software License Agreement
 * with HYGON.  This header does *NOT* give you permission to use the Materials
 * or any rights under HYGON's intellectual property.  Your use of any portion
 * of these Materials shall constitute your acceptance of those terms and
 * conditions.  If you do not agree to the terms and conditions of the Software
 * License Agreement, please do not use any portion of these Materials.
 * 
 * CONFIDENTIALITY:  The Materials and all other information, identified as
 * confidential and provided to you by HYGON shall be kept confidential in
 * accordance with the terms and conditions of the Software License Agreement.
 * 
 * LIMITATION OF LIABILITY: THE MATERIALS AND ANY OTHER RELATED INFORMATION
 * PROVIDED TO YOU BY HYGON ARE PROVIDED "AS IS" WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTY OF ANY KIND, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
 * MERCHANTABILITY, NONINFRINGEMENT, TITLE, FITNESS FOR ANY PARTICULAR PURPOSE,
 * OR WARRANTIES ARISING FROM CONDUCT, COURSE OF DEALING, OR USAGE OF TRADE.
 * IN NO EVENT SHALL HYGON OR ITS LICENSORS BE LIABLE FOR ANY DAMAGES WHATSOEVER
 * (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF PROFITS, BUSINESS
 * INTERRUPTION, OR LOSS OF INFORMATION) ARISING OUT OF HYGON'S NEGLIGENCE,
 * GROSS NEGLIGENCE, THE USE OF OR INABILITY TO USE THE MATERIALS OR ANY OTHER
 * RELATED INFORMATION PROVIDED TO YOU BY HYGON, EVEN IF HYGON HAS BEEN ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGES.  BECAUSE SOME JURISDICTIONS PROHIBIT THE
 * EXCLUSION OR LIMITATION OF LIABILITY FOR CONSEQUENTIAL OR INCIDENTAL DAMAGES,
 * THE ABOVE LIMITATION MAY NOT APPLY TO YOU.
 * 
 * HYGON does not assume any responsibility for any errors which may appear in
 * the Materials or any other related information provided to you by HYGON, or
 * result from use of the Materials or any related information.
 * 
 * You agree that you will not reverse engineer or decompile the Materials.
 * 
 * NO SUPPORT OBLIGATION: HYGON is not obligated to furnish, support, or make any
 * further information, software, technical information, know-how, or show-how
 * available to you.  Additionally, HYGON retains the right to modify the
 * Materials at any time, without notice, and is not obligated to provide such
 * modified Materials to you.
 * 
 * AMD GRANT HYGON DECLARATION: ADVANCED MICRO DEVICES, INC.(AMD) granted HYGON has
 * the right to redistribute HYGON's Agesa version to BIOS Vendors and HYGON has
 * the right to make the modified version available for use with HYGON's PRODUCT.
 ******************************************************************************
 */
/*++
Module Name:

  AmdMemPei.c
  Init Memory interface

Abstract:
--*/

#include "PiPei.h"
#include <Library/BaseMemoryLib.h>
#include <Library/BaseLib.h>
#include <Ppi/AmdMemPpi.h>
#include <Library/AmdMemBaseLib/AmdMemBaseLib.h>
#include <Uefi.h>
#include <Library/AmdBaseLib.h>
#include <Library/IdsLib.h>
#include <Library/AmdMemMainLib/AmdMemMainLib.h>
#include <Library/AmdMemServicesLib.h>
#include "Filecode.h"

#define FILECODE MEM_AMDMEMPEI_AMDMEMPEI_FILECODE

STATIC PEI_INITIALIZE_AGESA_MEMORY_PPI mInitializeAgesaMemoryPpi = {
  AMD_MEMORY_INITIALIZE_AGESA_MEMORY_REVISION
};

STATIC EFI_PEI_PPI_DESCRIPTOR mInitializeAgesaMemoryPiList =
{
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gAmdMemoryInitializeAgesaMemoryPpiGuid,
  &mInitializeAgesaMemoryPpi
};

STATIC PEI_MEMORY_INIT_COMPLETE_PPI mMemoryInitCompletePpi = {
  AMD_MEMORY_INIT_COMPLETE_REVISION,
  AmdGetMemoryAbove4gbService,
  AmdGetMemoryBelow4gbService,
  AmdGetMemoryBelow1TbService,
  AmdGetBottomIoService,
  AmdGetTotalMemorySizeService,
  AmdGetMemoryFrequencyService,
  AmdGetUmaInfoService
};

STATIC EFI_PEI_PPI_DESCRIPTOR mMemoryInitCompleteiList =
{
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gAmdMemoryInitCompletePpiGuid,
  &mMemoryInitCompletePpi
};

STATIC AMD_MEM_POOL_PTR_TABLE_PPI mMemPoolPtrTablePpi = {
  AMD_MEM_POOL_PTR_TABLE_REVISION
};

STATIC EFI_PEI_PPI_DESCRIPTOR mMemPolPtrTablePiList =
{
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gAmdMemoryPoolPointerTablePpiGuid,
  &mMemPoolPtrTablePpi
};
/*++

Routine Description:

  PSP Driver Entry. Initialize PSP device and report DRAM info to PSP once found

Arguments:

Returns:

  EFI_STATUS

--*/
EFI_STATUS
EFIAPI
PeiAmdMemEntry (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS            Status = EFI_SUCCESS;
  MEM_DATA_STRUCT       *MemDataStructure;
  //PEI_AMD_SOC_ID_CHECK_PPI *AmdSocIdPpiPtr;
  AMD_MEM_POOL_PTR_TABLE_PPI *AmdPoolTablePtr;
  IN AMD_CONFIG_PARAMS  *StdHeader;

  //-----------------------------------------
  //
  //              Check SOC ID
  //
  //-----------------------------------------
  //Status = (*PeiServices)->LocatePpi (PeiServices,
  //                                    &AmdSocIdCheckPpi,
  //                                    0,
  //                                    NULL,
  //                                    &AmdSocIdPpiPtr);
  //if (EFI_SUCCESS != Status) {
  //  // return error message.
  //  return Status;
  //}
  //Status = AmdSocIdPpiPtr->CheckSocId (PeiServices, SocId);
  //if (Status != EFI_SUCCESS) {
  //  return EFI_NOT_FOUND;
  //}

  StdHeader = NULL;
  AGESA_TESTPOINT (TpProcMemAmdMemAuto, StdHeader);
  //----------------------------------------------------------------
  //
  // Initialize Pool Pointer table - keep track of pointers allocated form the pool
  //
  //----------------------------------------------------------------
  Status = (**PeiServices).InstallPpi (PeiServices, &mMemPolPtrTablePiList);
  if (EFI_SUCCESS != Status) {
    return Status;
  }
  //----------------------------------------------------------------
  //
  //            Allocate space for data structure
  //
  //----------------------------------------------------------------
  Status = (*PeiServices)->AllocatePool (
                           PeiServices,
                           sizeof (MEM_DATA_STRUCT),
                           &MemDataStructure
                           );
  if (EFI_SUCCESS != Status) {
    return Status;
  }
  //----------------------------------------------------------------
  //
  //            Save allocate space for data structure
  //
  //----------------------------------------------------------------
  Status = (*PeiServices)->LocatePpi (PeiServices,
                                    &gAmdMemoryPoolPointerTablePpiGuid,
                                    0,
                                    NULL,
                                    &AmdPoolTablePtr);
  if (EFI_SUCCESS != Status) {
    return Status;
  }
  AmdPoolTablePtr->PtrTable[AMD_MEM_DATA_STRUCT_POOL_PTR] = (UINT32*)MemDataStructure;

  //----------------------------------------------------------------
  //
  //            Initialize PCDs values in memory data structure
  //
  //----------------------------------------------------------------

  // Initialize Parameters
  // MemMainInitParameters(MemDataStructure);

  //----------------------------------------------------------------
  //
  //            Initialize Memory
  //
  //----------------------------------------------------------------
  IDS_HDT_CONSOLE (MAIN_FLOW, "Memory Driver\n");

  //----------------------------------------------------------------
  //
  //            Initiate Program specific Memory initialization
  //
  //----------------------------------------------------------------
  Status = (**PeiServices).InstallPpi (PeiServices, &mInitializeAgesaMemoryPiList);
  if (EFI_SUCCESS != Status) {
    return Status;
  }
  //----------------------------------------------------------------
  //
  //            De-allocate space for data structure
  //
  //----------------------------------------------------------------
  AmdPoolTablePtr->PtrTable[AMD_MEM_DATA_STRUCT_POOL_PTR] = (UINT32*)NULL;
  //----------------------------------------------------------------
  //
  //          Indicate that Memory initialization complete
  //
  //----------------------------------------------------------------
  Status = (**PeiServices).InstallPpi (PeiServices, &mMemoryInitCompleteiList);
  if (EFI_SUCCESS != Status) {
    return Status;
  }
  AGESA_TESTPOINT (TpProcMemEnd, StdHeader);
  return (Status);
}

