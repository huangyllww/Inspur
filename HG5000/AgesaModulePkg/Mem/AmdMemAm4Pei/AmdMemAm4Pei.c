/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Memory API, and related functions.
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

  AmdMemAm4Pei.c
  Init Memory interface

Abstract:
--*/

#include "PiPei.h"
#include <Library/AmdBaseLib.h>
#include <Library/IdsLib.h>
#include <Ppi/AmdMemPpi.h>
#include <Library/BaseMemoryLib.h>
#include <Library/BaseLib.h>
#include <Library/AmdPspApobLib.h>
#include <Library/HobLib.h>
#include "Filecode.h"

#define FILECODE MEM_AMDMEMAM4PEI_AMDMEMAM4PEI_FILECODE
extern EFI_GUID gAmdNvdimmInfoHobGuid;
VOID
STATIC
DumpBuffer (
    VOID        *Buffer,
    UINT32        Size
  )
{
  UINT32 i;
  UINT8 *BufPtr;

  IDS_HDT_CONSOLE (MAIN_FLOW, "Buffer:\n");
  BufPtr = (UINT8*) Buffer;

  IDS_HDT_CONSOLE (MAIN_FLOW, "00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15\n\n");
  for (i = 0; i < Size; i++, BufPtr++) {
    IDS_HDT_CONSOLE (MAIN_FLOW, "%02x ", *(BufPtr));
    if ((i != 0) && ((i + 1) % 16 == 0)) {
      IDS_HDT_CONSOLE (MAIN_FLOW, "\n");
    }
  }
  IDS_HDT_CONSOLE (MAIN_FLOW, "\n");
}
EFI_STATUS
GetSystemMemoryMapInternal (
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  IN UINT32                     *NumberOfHoles,
  IN UINT64                     *TopOfSystemMemory,
  IN  VOID                      **MemHoleDescPtr
  )
{
  EFI_STATUS                          Status;
  UINT32                              NumOfMemHoleMap;
  APOB_TYPE_HEADER                    *ApobEntry;
  APOB_SYSTEM_MEMORY_MAP_TYPE_STRUCT  *SystemMemoryMapPtr;
  APOB_SYSTEM_MEMORY_MAP_TYPE_STRUCT  SystemMemoryMap;

  Status = AmdPspGetApobEntryInstance (APOB_FABRIC, APOB_SYS_MAP_INFO_TYPE, 0, FALSE, &ApobEntry);
  if (Status != EFI_SUCCESS) {
    return Status;
  }
  CopyMem (&SystemMemoryMap, ApobEntry, sizeof (APOB_SYSTEM_MEMORY_MAP_TYPE_STRUCT));
  //
  // Get the number of holes so that you can allocate sufficient buffer for all entries
  //
  NumOfMemHoleMap = SystemMemoryMap.ApobSystemMap.NumberOfHoles;
  //
  // Allocate some buffer for Memory Hole Map Descriptor
  //
  if (NumOfMemHoleMap != 0) {
    Status = (*PeiServices)->AllocatePool (PeiServices,
                      (((NumOfMemHoleMap -1 ) * sizeof (MEMORY_HOLE_DESCRIPTOR)) + sizeof (APOB_SYSTEM_MEMORY_MAP_TYPE_STRUCT)),
                                         &SystemMemoryMapPtr);
    if (Status != EFI_SUCCESS) {
      return Status;
    }
    CopyMem (SystemMemoryMapPtr, ApobEntry, (((NumOfMemHoleMap -1 ) * sizeof (MEMORY_HOLE_DESCRIPTOR)) + sizeof (APOB_SYSTEM_MEMORY_MAP_TYPE_STRUCT)));
    *MemHoleDescPtr =  (VOID *)&SystemMemoryMapPtr->ApobSystemMap.HoleInfo[0];
    *TopOfSystemMemory = SystemMemoryMapPtr->ApobSystemMap.TopOfSystemMemory;
    *NumberOfHoles = NumOfMemHoleMap;
  }

  return EFI_SUCCESS;
}
EFI_STATUS
GetDdrPostPackageRepair (
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  IN  VOID                      **MemDdrPostPackageRepairPtr
  )
{
  EFI_STATUS                          Status;
  APOB_DPPR_STRUCT                    *PostPackageRepairPtr;
  APOB_TYPE_HEADER                    *ApobEntry;
  APOB_MEM_GENERAL_CONFIGURATION_INFO_TYPE_STRUCT   ApobMemGenConfig;
  IDS_HDT_CONSOLE (MAIN_FLOW, "Get PPR Data\n");

  Status = (*PeiServices)->AllocatePool (PeiServices, sizeof (APOB_DPPR_STRUCT),
                                         &PostPackageRepairPtr);
  if (Status != EFI_SUCCESS) {
    return Status;
  }
  Status = AmdPspGetApobEntryInstance (APOB_MEM, APOB_MEM_GENERAL_CONFIGURATION_INFO_TYPE, 0, FALSE, &ApobEntry);
  if (Status != EFI_SUCCESS) {
    return Status;
  }
  CopyMem (&ApobMemGenConfig, ApobEntry, sizeof (APOB_MEM_GENERAL_CONFIGURATION_INFO_TYPE_STRUCT));
  CopyMem (PostPackageRepairPtr, &ApobMemGenConfig.DdrPostPackageRepair, sizeof (APOB_DPPR_STRUCT));
  *MemDdrPostPackageRepairPtr =  (VOID *)PostPackageRepairPtr; 
  IDS_HDT_CONSOLE (MAIN_FLOW, "Check APOB PostPackageRepair for PPR Data\n");  
  DumpBuffer (PostPackageRepairPtr, sizeof (APOB_DPPR_STRUCT));
  IDS_HDT_CONSOLE (MAIN_FLOW, "Dump Done PPR Data\n");
  if (PostPackageRepairPtr->PprResultsValid == TRUE) {
    IDS_HDT_CONSOLE (MAIN_FLOW, "PPR valid\n");  
    return EFI_SUCCESS;
  } else {
    IDS_HDT_CONSOLE (MAIN_FLOW, "PPR invalid\n");  
    return EFI_NOT_FOUND;
  }
}
/*++

Routine Description:

  PSP Driver Entry. Initialize PSP device and report DRAM info to PSP once found

Arguments:

Returns:

  EFI_STATUS

--*/
EFI_STATUS
EFIAPI
AmdMemAm4PeimZp (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS                      Status;
  AMD_MEMORY_INIT_COMPLETE_PPI    *MemInitCompleteData;
  EFI_PEI_PPI_DESCRIPTOR          *MemInitPpiList;
  APOB_TYPE_HEADER                *ApobEntry;
  APOB_MEM_GENERAL_CONFIGURATION_INFO_TYPE_STRUCT   ApobMemGenConfig;
  APOB_MEM_NVDIMM_INFO_STRUCT         NvdimmMemoryInfo;
  IN  VOID                      *MemDdrPostPackageRepairPtr;

  APOB_MEM_DIMM_SPD_DATA_STRUCT ApobSpdHeader;
  UINT8                 Socket;
  UINT8                 Channel;
  UINT16                Index;
  UINT8                 Dimm;
  UINT16                Instance;
  UINT16                DieLoop;
  UINT16                dimmIndex;

  APOB_APCB_BOOT_INFO_STRUCT *ApobEntry2;
    
  Status = AmdPspGetApobEntryInstance (APOB_MEM, APOB_APCB_BOOT_INFO_TYPE, 0, FALSE, (APOB_TYPE_HEADER **) &ApobEntry2);

  if(ApobEntry2->ApcbRecoveryFlag)
  {
	  //Improvement: Move outside AGESA, check if 1st boot after flash and then clear recovery flag if set.
	  //ApobEntry2->ApcbRecoveryFlag = 0;
  }
  
    ApobEntry = NULL;
	
  //----------------------------------------------------------------
  //
  // Allocate space for AmdMemoryInitCompletePpi
  //
  //----------------------------------------------------------------
  Status = (*PeiServices)->AllocatePool (PeiServices, sizeof (AMD_MEMORY_INIT_COMPLETE_PPI),
                                         &MemInitCompleteData);

  if (Status != EFI_SUCCESS) {
    return Status;
  }
  IDS_HDT_CONSOLE (MAIN_FLOW, "AGESA Memory Driver\n");
  ZeroMem (MemInitCompleteData, sizeof (AMD_MEMORY_INIT_COMPLETE_PPI));
  MemInitCompleteData->Revision = AMD_MEMORY_INIT_COMPLETE_REVISION;

  Status = AmdPspGetApobEntryInstance (APOB_MEM, APOB_MEM_GENERAL_CONFIGURATION_INFO_TYPE, 0, FALSE, &ApobEntry);

  if ((Status != EFI_SUCCESS) || (ApobEntry == NULL)) {
    MemInitCompleteData->AmdMemoryFrequency = 0;
    MemInitCompleteData->DdrMaxRate = 0;
  } else {
    CopyMem (&ApobMemGenConfig, ApobEntry, sizeof (APOB_MEM_GENERAL_CONFIGURATION_INFO_TYPE_STRUCT));
    MemInitCompleteData->AmdMemoryFrequency = ApobMemGenConfig.MemClkFreq;
    MemInitCompleteData->DdrMaxRate = ApobMemGenConfig.DdrMaxRate;

  }
  //----------------------------------------------------------------
  //
  // Update with SPD data (If supported)
  //
  //----------------------------------------------------------------
  // Initialize SPD data
  for (Socket = 0; Socket < AMD_MEM_PPI_MAX_SOCKETS_SUPPORTED; Socket++) {
    for (Channel = 0; Channel < AMD_MEM_PPI_MAX_CHANNELS_PER_SOCKET; Channel++) {
      for (Dimm = 0; Dimm < AMD_MEM_PPI_MAX_DIMMS_PER_CHANNEL; Dimm++) {
        dimmIndex = (Socket * AMD_MEM_PPI_MAX_CHANNELS_PER_SOCKET * AMD_MEM_PPI_MAX_DIMMS_PER_CHANNEL) + (Channel* AMD_MEM_PPI_MAX_DIMMS_PER_CHANNEL) + Dimm;
        MemInitCompleteData->AmdDimmSpInfo.DimmSpdInfo[dimmIndex].SocketNumber = Socket;
        MemInitCompleteData->AmdDimmSpInfo.DimmSpdInfo[dimmIndex].ChannelNumber = Channel;
        MemInitCompleteData->AmdDimmSpInfo.DimmSpdInfo[dimmIndex].DimmNumber = Dimm;
        MemInitCompleteData->AmdDimmSpInfo.DimmSpdInfo[dimmIndex].DimmPresent = FALSE;
        MemInitCompleteData->AmdDimmSpInfo.DimmSpdInfo[dimmIndex].MuxPresent = 0;
        MemInitCompleteData->AmdDimmSpInfo.DimmSpdInfo[dimmIndex].Address = 0;
        MemInitCompleteData->AmdDimmSpInfo.DimmSpdInfo[dimmIndex].SerialNumber = 0;
        for (Index = 0; Index < 512; Index++) {
          MemInitCompleteData->AmdDimmSpInfo.DimmSpdInfo[Socket*2 + Dimm + Channel*2].Data[Index] = 0;
        }
      }
    }
  }
  DieLoop = 0;
  // Update SPD data
  for (Socket = 0; Socket < AMD_MEM_PPI_MAX_SOCKETS_SUPPORTED; Socket++) {
    //Instance = (Channel >> 1);
    Instance = DieLoop;
    Instance |= ((Socket & 0x000000FF) << 8);
    IDS_HDT_CONSOLE (MAIN_FLOW, "Get Spd Data from APOB for Socket %d\n", Socket);
    for (DieLoop = 0; DieLoop < AMD_MEM_PPI_MAX_DIES_PER_SOCKET; DieLoop++) {
      Instance = DieLoop;
      Instance |= ((Socket & 0x000000FF) << 8);
      Status = AmdPspGetApobEntryInstance (APOB_MEM, APOB_MEM_DIMM_SPD_DATA_TYPE, Instance, FALSE, &ApobEntry);
      if (Status == EFI_SUCCESS) {
        //Instance |= (Channel >> 1);
        CopyMem (&ApobSpdHeader, ApobEntry, sizeof (APOB_MEM_DIMM_SPD_DATA_STRUCT));
        for (Channel = 0; Channel < ApobSpdHeader.MaxChannelsPerSocket; Channel++) {
          IDS_HDT_CONSOLE (MAIN_FLOW, "Get Spd Data from APOB for Channel %d\n", Channel);
          for (Dimm = 0; Dimm < ApobSpdHeader.MaxDimmsPerChannel; Dimm++) {
            IDS_HDT_CONSOLE (MAIN_FLOW, "Get Spd Data from APOB for Dimm %d\n", Dimm);
            if (ApobSpdHeader.DimmSmbusInfo[Dimm + Channel*2].DimmPresent == TRUE) {
              dimmIndex = (Socket * AMD_MEM_PPI_MAX_CHANNELS_PER_SOCKET * AMD_MEM_PPI_MAX_DIMMS_PER_CHANNEL) +
                (DieLoop * AMD_MEM_PPI_MAX_CHANNELS_PER_DIE * AMD_MEM_PPI_MAX_DIMMS_PER_CHANNEL) + (Channel * AMD_MEM_PPI_MAX_DIMMS_PER_CHANNEL) + Dimm;
              MemInitCompleteData->AmdDimmSpInfo.DimmSpdInfo[dimmIndex].DimmPresent = ApobSpdHeader.DimmSmbusInfo[Dimm + Channel*2].DimmPresent;
              MemInitCompleteData->AmdDimmSpInfo.DimmSpdInfo[dimmIndex].Address = ApobSpdHeader.DimmSmbusInfo[Dimm + Channel*2].Address;
              MemInitCompleteData->AmdDimmSpInfo.DimmSpdInfo[dimmIndex].MuxI2CAddress = ApobSpdHeader.DimmSmbusInfo[Dimm + Channel*2].MuxI2CAddress;
              MemInitCompleteData->AmdDimmSpInfo.DimmSpdInfo[dimmIndex].MuxPresent = ApobSpdHeader.DimmSmbusInfo[Dimm + Channel*2].MuxPresent;
              MemInitCompleteData->AmdDimmSpInfo.DimmSpdInfo[dimmIndex].MuxChannel = ApobSpdHeader.DimmSmbusInfo[Dimm + Channel*2].MuxChannel;
              MemInitCompleteData->AmdDimmSpInfo.DimmSpdInfo[dimmIndex].SerialNumber = ApobSpdHeader.DimmSmbusInfo[Dimm + Channel*2].SerialNumber;
              IDS_HDT_CONSOLE (MAIN_FLOW, "Buffer Check before\n");
              DumpBuffer (&MemInitCompleteData->AmdDimmSpInfo.DimmSpdInfo[Socket*2 + DieLoop*4 + Dimm + Channel*2].Data[0], 512);
              IDS_HDT_CONSOLE (MAIN_FLOW, "Buffer Check After\n");
              for (Index = 0; Index < 512; Index++) {
                MemInitCompleteData->AmdDimmSpInfo.DimmSpdInfo[Socket*2 + DieLoop*4 + Dimm + Channel*2].Data[Index] = ApobSpdHeader.DimmSmbusInfo[Dimm + Channel*2].Data[Index];
              }
              IDS_HDT_CONSOLE_PSP_TRACE ("Buffer Check Before Update\n");
              DumpBuffer (&MemInitCompleteData->AmdDimmSpInfo.DimmSpdInfo[Socket*2 + DieLoop*4 + Dimm + Channel*2].Data[0], 512);
              IDS_HDT_CONSOLE (MAIN_FLOW, "Buffer Check After Update\n");
            }
          }
        }
      }
    }
  }

  Status = (*PeiServices)->AllocatePool (PeiServices, sizeof (EFI_PEI_PPI_DESCRIPTOR), &MemInitPpiList);
  MemInitPpiList->Flags = (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST);
  MemInitPpiList->Guid  = &gAmdMemoryInitCompletePpiGuid;
  MemInitPpiList->Ppi   = MemInitCompleteData;
  ((AMD_MEMORY_INIT_COMPLETE_PPI *)MemInitPpiList->Ppi)->GetSystemMemoryMap = GetSystemMemoryMapInternal;
  ((AMD_MEMORY_INIT_COMPLETE_PPI *)MemInitPpiList->Ppi)->GetDdrPostPackageRepairInfo = GetDdrPostPackageRepair;

  Status = (*PeiServices)->InstallPpi (PeiServices, MemInitPpiList);
  //Test
  IDS_HDT_CONSOLE (MAIN_FLOW, "Call PPR Data\n");
  Status = ((AMD_MEMORY_INIT_COMPLETE_PPI *)MemInitPpiList->Ppi)->GetDdrPostPackageRepairInfo (PeiServices, &MemDdrPostPackageRepairPtr);
  IDS_HDT_CONSOLE (MAIN_FLOW, "Return from PPR Data\n");
  //Test

  Status = AmdPspGetApobEntryInstance (APOB_MEM, APOB_MEM_NVDIMM_INFO_TYPE, 0, FALSE, &ApobEntry);
  if (Status == EFI_SUCCESS) {
    CopyMem(&NvdimmMemoryInfo, ApobEntry, sizeof(APOB_MEM_NVDIMM_INFO_STRUCT));
    if (BuildGuidDataHob (&gAmdNvdimmInfoHobGuid, &NvdimmMemoryInfo.NvdimmInfo[0][0], (sizeof (NvdimmMemoryInfo.NvdimmInfo) + sizeof (NvdimmMemoryInfo.NvdimmPresentInSystem))) == NULL) {
      Status = EFI_NOT_FOUND;
    }
  }

  return (Status);
}

