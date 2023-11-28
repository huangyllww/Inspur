/* $NoKeywords:$ */
/**
 * @file
 *
 * AmdMemoryInfoHobPeim.
 *
 * Contains code that create Memory Hob.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  UEFI
 * @e \$Revision: 320059 $   @e \$Date: 2015-06-04 00:21:59 -0500 (Thu, 04 Jun 2015) $
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
 * government nor will be used for any purpose prohibited by the same.
 ******************************************************************************
 */
/*++
Module Name:

  AmdMemoryInfoHobPeim.c

Abstract:
--*/

#include <Library/HobLib.h>
#include <Library/BaseMemoryLib.h>
#include <Guid/AmdMemoryInfoHob.h>
#include <Ppi/AmdMemoryInfoHobPpi.h>
#include <Ppi/AmdMemPpi.h>
#include <Library/AmdPspApobLib.h>
#include <APOB.h>
#include <Library/IdsLib.h>

#include "Filecode.h"
#define FILECODE UNIVERSAL_AMDMEMORYHOBINFOPEIM_AMDMEMORYHOBINFOPEIM_FILECODE

extern EFI_GUID gAmdMemoryInitCompletePpiGuid;
extern EFI_GUID gAmdNvdimmInfoHobGuid;
typedef struct _MEMORY_INFO_BLOCK_STRUCT {
  IN BOOLEAN           MemFrequencyValid;   ///< Memory Frequency Valid
  IN UINT32            MemFrequency;        ///< Memory Frequency
  IN BOOLEAN           VddioValid;          ///< This field determines if Vddio is valid
  IN UINT16            Vddio;               ///< Vddio Voltage
  IN BOOLEAN           VddpVddrValid;       ///< This field determines if VddpVddr is valid
  IN UINT8             VddpVddr;            ///< VddpVddr voltage
  IN UINT32            DdrMaxRate;          ///< UMC DdrMaxRateVddpVddr
} MEMORY_INFO_BLOCK_STRUCT;

#ifndef FOURGB
#define FOURGB      0x100000000ull
#endif

#define MAX_NUMBER_OF_EXTENDED_MEMORY_DESCRIPTOR    19
#define MAX_SIZEOF_AMD_MEMORY_INFO_HOB_BUFFER       (sizeof (AMD_MEMORY_INFO_HOB) +  \
          (MAX_NUMBER_OF_EXTENDED_MEMORY_DESCRIPTOR * sizeof (AMD_MEMORY_RANGE_DESCRIPTOR)))


//
// PPI Initialization
//
STATIC AMD_MEMORY_INFO_HOB_PPI mAmdMemoryHobInfoAvailblePpi = {
  AMD_MEMORY_INFO_HOB_PPI_REVISION
};


STATIC EFI_PEI_PPI_DESCRIPTOR mAmdMemoryHobInfoAvailblePpiList =
{
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gAmdMemoryInfoHobPpiGuid,
  &mAmdMemoryHobInfoAvailblePpi
};

EFI_STATUS
BuildHobInfo (
  IN CONST  EFI_PEI_SERVICES    **PeiServices,
  IN MEMORY_INFO_BLOCK_STRUCT   *MemoInfoBlockPtr
  );

EFI_STATUS
BuildHobInfo (
  IN CONST  EFI_PEI_SERVICES    **PeiServices,
  IN MEMORY_INFO_BLOCK_STRUCT   *MemInfoBlockPtr
  )
/*++

Routine Description:

  This function build HOB info from post interface parameters

Arguments:

  PeiServices     -     PeiServices
  MemInfoBlockPtr -     MemInfoBlock pointer

Returns:
  EFI_STATUS - Status code
--*/
{
  EFI_STATUS                          Status;
  UINT32                              Index;
  UINT64                              TopOfMemAddress;
  UINT64                              CurrentBase;
  UINT32                              NumOfHoles;
  UINT8                               MemInfoHobBuffer[MAX_SIZEOF_AMD_MEMORY_INFO_HOB_BUFFER];
  AMD_MEMORY_INFO_HOB                 *MemInfoHob;
  AMD_MEMORY_RANGE_DESCRIPTOR         *MemRangeDesc;
  UINTN                               SizeOfMemInfoHob;
  UINT32                              MemRangeIndex;
  AMD_MEMORY_INIT_COMPLETE_PPI        *mMemoryInitCompletePpiPtr;
  MEMORY_HOLE_DESCRIPTOR              *HoleMapPtr;

  HoleMapPtr = NULL;
  Status = (*PeiServices)->LocatePpi (PeiServices,
                                      &gAmdMemoryInitCompletePpiGuid,
                                      0,
                                      NULL,
                                      &mMemoryInitCompletePpiPtr);
  if (EFI_SUCCESS != Status) {
    return Status;
  }
  Status = mMemoryInitCompletePpiPtr->GetSystemMemoryMap (
    PeiServices,
    &NumOfHoles,
    &TopOfMemAddress,
    &HoleMapPtr
    );
  if (EFI_SUCCESS != Status) {
    return Status;
  }
  if (HoleMapPtr == NULL) {
    return EFI_NOT_FOUND;
  }

  MemInfoHob = (AMD_MEMORY_INFO_HOB *) &MemInfoHobBuffer[0];
  MemRangeDesc = &MemInfoHob->Ranges[0];
  MemInfoHob->Version = AMD_MEMORY_INFO_HOB_VERISION;
  MemRangeIndex = 0;
  CurrentBase = 0;
  IDS_HDT_CONSOLE (MAIN_FLOW, "    NumOfHoles: %d\n", NumOfHoles);
  IDS_HDT_CONSOLE (MAIN_FLOW, "    HoleMapPtr: %x\n", HoleMapPtr);
  for (Index = 0; Index < NumOfHoles; Index++) {
    switch (HoleMapPtr->Type) {
    case MMIO:
      MemRangeDesc[MemRangeIndex].Size =  (HoleMapPtr->Base - CurrentBase);
      if ( 0 != MemRangeDesc[MemRangeIndex].Size) {
        MemRangeDesc[MemRangeIndex].Attribute = AMD_MEMORY_ATTRIBUTE_AVAILABLE;
        MemRangeDesc[MemRangeIndex].Base = CurrentBase;
        CurrentBase += MemRangeDesc[MemRangeIndex].Size;

        IDS_HDT_CONSOLE (MAIN_FLOW, "    MemRangeIndex: 0x%x\n", MemRangeIndex);
        IDS_HDT_CONSOLE (MAIN_FLOW, "    Base Hi: 0x%08x\n", RShiftU64 (MemRangeDesc[MemRangeIndex].Base, 32));
        IDS_HDT_CONSOLE (MAIN_FLOW, "    Base Lo: 0x%08x\n", ((MemRangeDesc[MemRangeIndex].Base) & 0xFFFFFFFF));
        IDS_HDT_CONSOLE (MAIN_FLOW, "    Size Hi: 0x%08x\n", RShiftU64 (MemRangeDesc[MemRangeIndex].Size, 32));
        IDS_HDT_CONSOLE (MAIN_FLOW, "    Size Lo: 0x%08x\n", ((MemRangeDesc[MemRangeIndex].Size) & 0xFFFFFFFF));
        IDS_HDT_CONSOLE (MAIN_FLOW, "    Attribute: 0x%08x\n", MemRangeDesc[MemRangeIndex].Attribute);
        IDS_HDT_CONSOLE (MAIN_FLOW, "    CurrentBase Hi: 0x%08x\n", RShiftU64(CurrentBase, 32));
        IDS_HDT_CONSOLE (MAIN_FLOW, "    CurrentBase Lo: 0x%08x\n", (CurrentBase & 0xFFFFFFFF));
        IDS_HDT_CONSOLE (MAIN_FLOW, "\n");

        MemRangeIndex += 1;
      }

      MemRangeDesc[MemRangeIndex].Size =  HoleMapPtr->Size;
      ASSERT (0 != MemRangeDesc[MemRangeIndex].Size);
      MemRangeDesc[MemRangeIndex].Attribute = AMD_MEMORY_ATTRIBUTE_MMIO;
      MemRangeDesc[MemRangeIndex].Base = HoleMapPtr->Base;
      CurrentBase += HoleMapPtr->Size;
      IDS_HDT_CONSOLE (MAIN_FLOW, "    MemRangeIndex: 0x%x\n", MemRangeIndex);
      IDS_HDT_CONSOLE (MAIN_FLOW, "    Base Hi: 0x%08x\n", RShiftU64 (MemRangeDesc[MemRangeIndex].Base, 32));
      IDS_HDT_CONSOLE (MAIN_FLOW, "    Base Lo: 0x%08x\n", ((MemRangeDesc[MemRangeIndex].Base) & 0xFFFFFFFF));
      IDS_HDT_CONSOLE (MAIN_FLOW, "    Size Hi: 0x%08x\n", RShiftU64 (MemRangeDesc[MemRangeIndex].Size, 32));
      IDS_HDT_CONSOLE (MAIN_FLOW, "    Size Lo: 0x%08x\n", ((MemRangeDesc[MemRangeIndex].Size) & 0xFFFFFFFF));
      IDS_HDT_CONSOLE (MAIN_FLOW, "    Attribute: 0x%08x\n", MemRangeDesc[MemRangeIndex].Attribute);
      IDS_HDT_CONSOLE (MAIN_FLOW, "    CurrentBase Hi: 0x%08x\n", RShiftU64(CurrentBase, 32));
      IDS_HDT_CONSOLE (MAIN_FLOW, "    CurrentBase Lo: 0x%08x\n", (CurrentBase & 0xFFFFFFFF));
      IDS_HDT_CONSOLE (MAIN_FLOW, "\n");
      break;

    case PrivilegedDRAM:
      MemRangeDesc[MemRangeIndex].Size =  (HoleMapPtr->Base - CurrentBase);
      if (0 != MemRangeDesc[MemRangeIndex].Size) {
        MemRangeDesc[MemRangeIndex].Attribute = AMD_MEMORY_ATTRIBUTE_AVAILABLE;
        MemRangeDesc[MemRangeIndex].Base = CurrentBase;
        CurrentBase += MemRangeDesc[MemRangeIndex].Size; 

        IDS_HDT_CONSOLE (MAIN_FLOW, "    MemRangeIndex: 0x%x\n", MemRangeIndex);
        IDS_HDT_CONSOLE (MAIN_FLOW, "    Base Hi: 0x%08x\n", RShiftU64 (MemRangeDesc[MemRangeIndex].Base, 32));
        IDS_HDT_CONSOLE (MAIN_FLOW, "    Base Lo: 0x%08x\n", ((MemRangeDesc[MemRangeIndex].Base) & 0xFFFFFFFF));
        IDS_HDT_CONSOLE (MAIN_FLOW, "    Size Hi: 0x%08x\n", RShiftU64 (MemRangeDesc[MemRangeIndex].Size, 32));
        IDS_HDT_CONSOLE (MAIN_FLOW, "    Size Lo: 0x%08x\n", ((MemRangeDesc[MemRangeIndex].Size) & 0xFFFFFFFF));
        IDS_HDT_CONSOLE (MAIN_FLOW, "    Attribute: 0x%08x\n", MemRangeDesc[MemRangeIndex].Attribute);
        IDS_HDT_CONSOLE (MAIN_FLOW, "    CurrentBase Hi: 0x%08x\n", RShiftU64(CurrentBase, 32));
        IDS_HDT_CONSOLE (MAIN_FLOW, "    CurrentBase Lo: 0x%08x\n", (CurrentBase & 0xFFFFFFFF));
        IDS_HDT_CONSOLE (MAIN_FLOW, "\n");

        MemRangeIndex += 1;
      }

      MemRangeDesc[MemRangeIndex].Size =  HoleMapPtr->Size;
      ASSERT (0 != MemRangeDesc[MemRangeIndex].Size);
      MemRangeDesc[MemRangeIndex].Attribute = AMD_MEMORY_ATTRIBUTE_RESERVED;
      MemRangeDesc[MemRangeIndex].Base = HoleMapPtr->Base;
      CurrentBase += HoleMapPtr->Size; 

      IDS_HDT_CONSOLE (MAIN_FLOW, "    MemRangeIndex: 0x%x\n", MemRangeIndex);
      IDS_HDT_CONSOLE (MAIN_FLOW, "    Base Hi: 0x%08x\n", RShiftU64 (MemRangeDesc[MemRangeIndex].Base, 32));
      IDS_HDT_CONSOLE (MAIN_FLOW, "    Base Lo: 0x%08x\n", ((MemRangeDesc[MemRangeIndex].Base) & 0xFFFFFFFF));
      IDS_HDT_CONSOLE (MAIN_FLOW, "    Size Hi: 0x%08x\n", RShiftU64 (MemRangeDesc[MemRangeIndex].Size, 32));
      IDS_HDT_CONSOLE (MAIN_FLOW, "    Size Lo: 0x%08x\n", ((MemRangeDesc[MemRangeIndex].Size) & 0xFFFFFFFF));
      IDS_HDT_CONSOLE (MAIN_FLOW, "    Attribute: 0x%08x\n", MemRangeDesc[MemRangeIndex].Attribute);
      IDS_HDT_CONSOLE (MAIN_FLOW, "    CurrentBase Hi: 0x%08x\n", RShiftU64(CurrentBase, 32));
      IDS_HDT_CONSOLE (MAIN_FLOW, "    CurrentBase Lo: 0x%08x\n", (CurrentBase & 0xFFFFFFFF));
      IDS_HDT_CONSOLE (MAIN_FLOW, "\n");
      break;

    case UMA:
      MemRangeDesc[MemRangeIndex].Size =  (HoleMapPtr->Base - CurrentBase);
      if (0 != MemRangeDesc[MemRangeIndex].Size) {
        MemRangeDesc[MemRangeIndex].Attribute = AMD_MEMORY_ATTRIBUTE_AVAILABLE;
        MemRangeDesc[MemRangeIndex].Base = CurrentBase;
        CurrentBase += MemRangeDesc[MemRangeIndex].Size;

        IDS_HDT_CONSOLE (MAIN_FLOW, "    MemRangeIndex: 0x%x\n", MemRangeIndex);
        IDS_HDT_CONSOLE (MAIN_FLOW, "    Base Hi: 0x%08x\n", RShiftU64 (MemRangeDesc[MemRangeIndex].Base, 32));
        IDS_HDT_CONSOLE (MAIN_FLOW, "    Base Lo: 0x%08x\n", ((MemRangeDesc[MemRangeIndex].Base) & 0xFFFFFFFF));
        IDS_HDT_CONSOLE (MAIN_FLOW, "    Size Hi: 0x%08x\n", RShiftU64 (MemRangeDesc[MemRangeIndex].Size, 32));
        IDS_HDT_CONSOLE (MAIN_FLOW, "    Size Lo: 0x%08x\n", ((MemRangeDesc[MemRangeIndex].Size) & 0xFFFFFFFF));
        IDS_HDT_CONSOLE (MAIN_FLOW, "    Attribute: 0x%08x\n", MemRangeDesc[MemRangeIndex].Attribute);
        IDS_HDT_CONSOLE (MAIN_FLOW, "    CurrentBase Hi: 0x%08x\n", RShiftU64(CurrentBase, 32));
        IDS_HDT_CONSOLE (MAIN_FLOW, "    CurrentBase Lo: 0x%08x\n", (CurrentBase & 0xFFFFFFFF));
        IDS_HDT_CONSOLE (MAIN_FLOW, "\n");

        MemRangeIndex += 1;
      }

      MemRangeDesc[MemRangeIndex].Size =  HoleMapPtr->Size;
      ASSERT (0 != MemRangeDesc[MemRangeIndex].Size);
      MemRangeDesc[MemRangeIndex].Attribute = AMD_MEMORY_ATTRIBUTE_UMA;
      MemRangeDesc[MemRangeIndex].Base = HoleMapPtr->Base;
      CurrentBase += HoleMapPtr->Size;
      IDS_HDT_CONSOLE (MAIN_FLOW, "    MemRangeIndex: 0x%x\n", MemRangeIndex);
      IDS_HDT_CONSOLE (MAIN_FLOW, "    Base Hi: 0x%08x\n", RShiftU64 (MemRangeDesc[MemRangeIndex].Base, 32));
      IDS_HDT_CONSOLE (MAIN_FLOW, "    Base Lo: 0x%08x\n", ((MemRangeDesc[MemRangeIndex].Base) & 0xFFFFFFFF));
      IDS_HDT_CONSOLE (MAIN_FLOW, "    Size Hi: 0x%08x\n", RShiftU64 (MemRangeDesc[MemRangeIndex].Size, 32));
      IDS_HDT_CONSOLE (MAIN_FLOW, "    Size Lo: 0x%08x\n", ((MemRangeDesc[MemRangeIndex].Size) & 0xFFFFFFFF));
      IDS_HDT_CONSOLE (MAIN_FLOW, "    Attribute: 0x%08x\n", MemRangeDesc[MemRangeIndex].Attribute);
      IDS_HDT_CONSOLE (MAIN_FLOW, "    CurrentBase Hi: 0x%08x\n", RShiftU64(CurrentBase, 32));
      IDS_HDT_CONSOLE (MAIN_FLOW, "    CurrentBase Lo: 0x%08x\n", (CurrentBase & 0xFFFFFFFF));
      IDS_HDT_CONSOLE (MAIN_FLOW, "\n");
      break;

    default:
      Status = EFI_UNSUPPORTED;
      break;
    }
    MemRangeIndex++;
    HoleMapPtr++;
  }

  if (CurrentBase < TopOfMemAddress) {
    //
    // MemRangeIndex will be incremented in the previous loop hence dont need to increment here
    //
    MemRangeDesc[MemRangeIndex].Attribute = AMD_MEMORY_ATTRIBUTE_AVAILABLE;
    MemRangeDesc[MemRangeIndex].Base = CurrentBase;
    MemRangeDesc[MemRangeIndex].Size =  TopOfMemAddress - CurrentBase;

    IDS_HDT_CONSOLE (MAIN_FLOW, "    MemRangeIndex: 0x%x\n", MemRangeIndex);
    IDS_HDT_CONSOLE (MAIN_FLOW, "    Base Hi: 0x%08x\n", RShiftU64 (MemRangeDesc[MemRangeIndex].Base, 32));
    IDS_HDT_CONSOLE (MAIN_FLOW, "    Base Lo: 0x%08x\n", ((MemRangeDesc[MemRangeIndex].Base) & 0xFFFFFFFF));
    IDS_HDT_CONSOLE (MAIN_FLOW, "    Size Hi: 0x%08x\n", RShiftU64 (MemRangeDesc[MemRangeIndex].Size, 32));
    IDS_HDT_CONSOLE (MAIN_FLOW, "    Size Lo: 0x%08x\n", ((MemRangeDesc[MemRangeIndex].Size) & 0xFFFFFFFF));
    IDS_HDT_CONSOLE (MAIN_FLOW, "    Attribute: 0x%04x\n", MemRangeDesc[MemRangeIndex].Attribute);
    IDS_HDT_CONSOLE (MAIN_FLOW, "\n");
  } else {
    // Since no additional descriptor required to be updated, decrease the
    // incremented number by 1 to ensure we report correct number of descriptor
    MemRangeIndex--;
  }

  MemInfoHob->NumberOfDescriptor = MemRangeIndex + 1;
  SizeOfMemInfoHob = sizeof (AMD_MEMORY_INFO_HOB) + (MemInfoHob->NumberOfDescriptor - 1) * sizeof (AMD_MEMORY_RANGE_DESCRIPTOR);
  IDS_HDT_CONSOLE (MAIN_FLOW, "    NumberOfDescriptor: 0x%x\n", MemInfoHob->NumberOfDescriptor);
  IDS_HDT_CONSOLE (MAIN_FLOW, "    SizeOfMemInfoHob: 0x%x\n", SizeOfMemInfoHob);

  //
  // Update Voltage Information.
  //
  MemInfoHob->AmdMemoryVddioValid = MemInfoBlockPtr->VddioValid;
  MemInfoHob->AmdMemoryVddio = MemInfoBlockPtr->Vddio;
  MemInfoHob->AmdMemoryVddpVddrValid = MemInfoBlockPtr->VddpVddrValid;
  MemInfoHob->AmdMemoryVddpVddr = MemInfoBlockPtr->VddpVddr;
  MemInfoHob->AmdMemoryFrequencyValid = MemInfoBlockPtr->MemFrequencyValid;
  MemInfoHob->AmdMemoryFrequency = MemInfoBlockPtr->MemFrequency;
  MemInfoHob->AmdMemoryDdrMaxRate = MemInfoBlockPtr->DdrMaxRate;

  Status =  EFI_SUCCESS;
  if (BuildGuidDataHob (&gAmdMemoryInfoHobGuid, &MemInfoHobBuffer, SizeOfMemInfoHob) == NULL) {
    Status = EFI_NOT_FOUND;
  }

  return Status;
}

EFI_STATUS
GetMemInfoBlockData (
  IN       CONST EFI_PEI_SERVICES         **PeiServices,
  IN OUT         MEMORY_INFO_BLOCK_STRUCT *MemInfoBlockPtr
  )
/*++ -----------------------------------------------------------------------------
  Routine Description:
    This function will get the APOB Data from APOB HOB

  Arguments:
    MemInfoBlockPtr   - Pointer to the AGESA_PSP_OUTPUT_BLOCK_STRUCT

  Returns:
    EFI_STATUS  - Status code
                  EFI_SUCCESS
 */
{
  EFI_STATUS                     Status;
  AMD_MEMORY_INIT_COMPLETE_PPI   *mMemoryInitCompletePpiPtr;

  Status = (*PeiServices)->LocatePpi (PeiServices,
                                      &gAmdMemoryInitCompletePpiGuid,
                                      0,
                                      NULL,
                                      &mMemoryInitCompletePpiPtr);
  if (EFI_SUCCESS != Status) {
    return Status;
  }


  MemInfoBlockPtr->MemFrequencyValid = TRUE;
  MemInfoBlockPtr->MemFrequency = mMemoryInitCompletePpiPtr->AmdMemoryFrequency;

  MemInfoBlockPtr->VddioValid = TRUE;
  MemInfoBlockPtr->Vddio = (UINT16) mMemoryInitCompletePpiPtr->AmdMemoryVddIo;

  MemInfoBlockPtr->VddpVddrValid = mMemoryInitCompletePpiPtr->AmdMemoryVddpVddr.IsValid;
  MemInfoBlockPtr->VddpVddr = mMemoryInitCompletePpiPtr->AmdMemoryVddpVddr.Voltage;
  MemInfoBlockPtr->DdrMaxRate = mMemoryInitCompletePpiPtr->DdrMaxRate;

  return EFI_SUCCESS;
}


EFI_STATUS
EFIAPI
InitializeAmdMemoryInfoHobPeim (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
/*++

Routine Description:

  Initialization Entry Point for AmdMemoryHobInfo PEIM

Arguments:
  FileHandle -  FileHandle
  PeiServices - PeiServices

Returns:
  EFI_STATUS  - Status code
                EFI_SUCCESS

--*/
{
  EFI_STATUS                 Status;
  MEMORY_INFO_BLOCK_STRUCT   MemInfoBlock;
  //UINT32  NumofEntry;
  //APOB_TYPE_HEADER *ApobEntries;

  //NumofEntry = 0;

  // Retrieve DIMM Smbus info log
  //Status = AmdPspGetApobEntryFromHob(APOB_MEM, APOB_MEM_DIMM_SMBUS_INFO_TYPE, &NumofEntry, &ApobEntries);
  //if (Status == EFI_SUCCESS) {
  //  BuildGuidDataHob (&gAmdMemoryDimmSmbusInfoHobGuid, ((UINT8 *) ApobEntries + sizeof (APOB_TYPE_HEADER)), (ApobEntries->TypeSize - sizeof (APOB_TYPE_HEADER)));
  //}

  //
  // Get Memory Info Block Data
  //
  Status = GetMemInfoBlockData (PeiServices, &MemInfoBlock);
  if (Status != EFI_SUCCESS) {
    return Status;
  }

  //
  // Build Memory Info Hob
  //
  Status = BuildHobInfo (PeiServices, &MemInfoBlock);

  if (Status == EFI_SUCCESS) {
    Status = (**PeiServices).InstallPpi (PeiServices, &mAmdMemoryHobInfoAvailblePpiList);
  }
  return Status;
}
