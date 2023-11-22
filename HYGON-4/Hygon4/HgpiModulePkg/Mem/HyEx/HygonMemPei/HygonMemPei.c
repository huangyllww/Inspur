/* $NoKeywords:$ */

/**
 * @file
 *
 * HYGON Memory API, and related functions.
 *
 * Contains code that initializes memory
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      HGPI
 * @e sub-project:  PSP
 *
 */
/*****************************************************************************
 *
 *
 * Copyright 2016 - 2023 CHENGDU HAIGUANG IC DESIGN CO., LTD. All Rights Reserved.
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
 *
 ******************************************************************************
 */
/*++
Module Name:

  HygonMemDhm1Pei.c
  Init Memory interface

Abstract:
--*/

#include "PiPei.h"
#include <Library/HygonBaseLib.h>
#include <Library/IdsLib.h>
#include <Ppi/HygonMemPpi.h>
#include <Library/BaseMemoryLib.h>
#include <Library/BaseLib.h>
#include <Library/HygonPspHpobLib.h>
#include <Library/HobLib.h>
#include <Library/BaseFabricTopologyLib.h>
#include <Library/HygonMemBaseLib.h>
#include <Library/IoLib.h>
#include "Filecode.h"
#include <Library/BaseFabricTopologyLib.h>
#include <Library/HygonSocBaseLib.h>

#define FILECODE  MEM_HYGONMEMHyExPEI_HYGONMEMHyExPEI_FILECODE
extern EFI_GUID  gHygonNvdimmInfoHobGuid;

void
SetDDRTypeToPcd (
  void
  )
{
  MEM_UMC_CONFIG  UmcConfig;

  UmcConfig = ReadUmcConfig ();

  if(UmcConfig.Field.DdrType == 0) {
    // DDR4
    PcdSetBoolS (PcdDDR5Enable, FALSE);
  } else if(UmcConfig.Field.DdrType == 1) {
    // DDR5
    PcdSetBoolS (PcdDDR5Enable, TRUE);
  }

  IDS_HDT_CONSOLE (MAIN_FLOW, "UmcConfig=%X, DDRType=%a\n", UmcConfig, (UmcConfig.Field.DdrType == 0) ? "DDR4" : "DDR5");
}

VOID
STATIC
DumpBuffer (
  VOID        *Buffer,
  UINT32        Size
  )
{
  UINT32  i;
  UINT8   *BufPtr;

  IDS_HDT_CONSOLE (MAIN_FLOW, "Buffer:\n");
  BufPtr = (UINT8 *)Buffer;

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
  UINT32                              Index;
  UINT32                              HTHolePos;
  UINT32                              NumOfMemHoleMap;
  UINT32                              NumOfOverrideMemHoleMap;
  UINT64                              HTBaseAddress  = 0xFD00000000;
  UINT64                              HTLimitAddress = 0xFFFFFFFFFF;
  HPOB_TYPE_HEADER                    *HpobEntry;
  HPOB_SYSTEM_MEMORY_MAP_TYPE_STRUCT_HYEX  *SystemMemoryMapPtr;
  HPOB_SYSTEM_MEMORY_MAP_TYPE_STRUCT_HYEX  SystemMemoryMap;
  MEMORY_HOLE_DESCRIPTOR              *HolePtr;
  MEMORY_HOLE_DESCRIPTOR              *NextPtr;
    
  Status = HygonPspGetHpobEntryInstance (HPOB_GROUP_FABRIC, HPOB_SYS_MAP_INFO_TYPE, 0, FALSE, &HpobEntry);
  if (Status != EFI_SUCCESS) {
    return Status;
  }

  CopyMem (&SystemMemoryMap, HpobEntry, sizeof (HPOB_SYSTEM_MEMORY_MAP_TYPE_STRUCT_HYEX));
  //
  // Get the number of holes so that you can allocate sufficient buffer for all entries
  //
  NumOfMemHoleMap = SystemMemoryMap.HpobSystemMap.NumberOfHoles;

  if (SystemMemoryMap.HpobSystemMap.TopOfSystemMemory >= HTBaseAddress) {
    NumOfOverrideMemHoleMap = NumOfMemHoleMap + 1;
  } else {
    NumOfOverrideMemHoleMap = NumOfMemHoleMap;
  }

  //
  // Allocate some buffer for Memory Hole Map Descriptor
  //
  if (NumOfMemHoleMap != 0) {
    Status = (*PeiServices)->AllocatePool (
                               PeiServices,
                               (((NumOfOverrideMemHoleMap -1) * sizeof (MEMORY_HOLE_DESCRIPTOR)) + sizeof (HPOB_SYSTEM_MEMORY_MAP_TYPE_STRUCT_HYEX)),
                               &SystemMemoryMapPtr
                               );
    if (Status != EFI_SUCCESS) {
      return Status;
    }

    CopyMem (SystemMemoryMapPtr, HpobEntry, (((NumOfMemHoleMap -1) * sizeof (MEMORY_HOLE_DESCRIPTOR)) + sizeof (HPOB_SYSTEM_MEMORY_MAP_TYPE_STRUCT_HYEX)));
    *MemHoleDescPtr    =  (VOID *)&SystemMemoryMapPtr->HpobSystemMap.HoleInfo[0];
    *TopOfSystemMemory = SystemMemoryMapPtr->HpobSystemMap.TopOfSystemMemory;
    *NumberOfHoles     = NumOfOverrideMemHoleMap;
  }

  IDS_HDT_CONSOLE (MAIN_FLOW, "TopOfSystemMemory = 0x%lx\n", *TopOfSystemMemory);
  IDS_HDT_CONSOLE (MAIN_FLOW, "NumberOfHoles = 0x%d\n", *NumberOfHoles);
  if (*TopOfSystemMemory >= HTBaseAddress) {
    HTHolePos = 0;
    HolePtr   = (MEMORY_HOLE_DESCRIPTOR *)(*MemHoleDescPtr);

    for (Index = 0; Index < NumOfMemHoleMap; Index++) {
      IDS_HDT_CONSOLE (MAIN_FLOW, "Index = %d, Base: 0x%lx\n", Index, HolePtr->Base);
      if (HolePtr->Base > HTBaseAddress) {
        HTHolePos = Index;
        break;
      }

      HolePtr++;
    }

    IDS_HDT_CONSOLE (MAIN_FLOW, "HTHolePos = %d\n", HTHolePos);
    HolePtr = (MEMORY_HOLE_DESCRIPTOR *)(*MemHoleDescPtr);

    if (HTHolePos == 0) {
      // No hole cover FD_0000_0000~FF_FFFF_FFFF, so set HT hole as the last descriptor
      HolePtr += NumOfMemHoleMap;
      HolePtr->Base = HTBaseAddress;
      HolePtr->Size = HTLimitAddress - HTBaseAddress + 1;
      HolePtr->Type = PrivilegedDRAM;
    } else {
      HolePtr += NumOfMemHoleMap;
      NextPtr  = HolePtr - 1;
      for (Index = NumOfMemHoleMap; Index > HTHolePos; Index--) {
        HolePtr->Base = NextPtr->Base;
        HolePtr->Size = NextPtr->Size;
        HolePtr->Type = NextPtr->Type;
        HolePtr--;
        NextPtr--;
      }

      HolePtr->Base = HTBaseAddress;
      HolePtr->Size = HTLimitAddress - HTBaseAddress + 1;
      HolePtr->Type = PrivilegedDRAM;
    }

    HolePtr = (MEMORY_HOLE_DESCRIPTOR *)(*MemHoleDescPtr);

    for (Index = 0; Index < NumOfOverrideMemHoleMap; Index++) {
      IDS_HDT_CONSOLE (MAIN_FLOW, "Hole index: %d    Base:0x%lx    Size:0x%lx    Type:%d\n", Index, HolePtr->Base, HolePtr->Size, HolePtr->Type);
      HolePtr++;
    }
  }

  return EFI_SUCCESS;
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
HygonMemHyExPeimEntry (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS                                           Status;
  HYGON_MEMORY_INIT_COMPLETE_PPI                       *MemInitCompleteData;
  EFI_PEI_PPI_DESCRIPTOR                               *MemInitPpiList;
  HPOB_TYPE_HEADER                                     *HpobEntry;
  HPOB_MEM_GENERAL_CONFIGURATION_INFO_TYPE_STRUCT_HYEX  HpobMemGenConfigHyEx;
  HPOB_MEM_NVDIMM_INFO_STRUCT_HYEX                      NvdimmMemoryInfo;
  HPOB_MEM_DIMM_SPD_DATA_STRUCT_HYEX                    HpobSpdHeaderHyEx;
  UINT8                                                Socket;
  UINT8                                                Channel;
  UINT8                                                Dimm;
  UINT32                                               Instance;
  UINT16                                               CddLoop;
  UINT16                                               DimmIndex;
  UINT16                                               ChannelIndex;
  UINTN                                                SocketNum;
  UINTN                                                CddsPresentSocket;
  UINT32                                               HpobInstanceId;
  UINT16                                               SpdSize;

  HpobEntry = NULL;

  SetDDRTypeToPcd ();
  SpdSize = PcdGetBool (PcdDDR5Enable) ? DDR5_SPD_SIZE : DDR4_SPD_SIZE;

  // ----------------------------------------------------------------
  //
  // Allocate space for HygonMemoryInitCompletePpi
  //
  // ----------------------------------------------------------------
  Status = (*PeiServices)->AllocatePool (PeiServices, sizeof (HYGON_MEMORY_INIT_COMPLETE_PPI), &MemInitCompleteData);
  if (Status != EFI_SUCCESS) {
    IDS_HDT_CONSOLE (MAIN_FLOW, "Allocate 0x%X bytes for HYGON_MEMORY_INIT_COMPLETE_PPI fail, %r exit\n", sizeof (HYGON_MEMORY_INIT_COMPLETE_PPI), Status);
    return Status;
  }

  IDS_HDT_CONSOLE (MAIN_FLOW, "HGPI Memory Driver\n");
  ZeroMem (MemInitCompleteData, sizeof (HYGON_MEMORY_INIT_COMPLETE_PPI));
  MemInitCompleteData->Revision = HYGON_MEMORY_INIT_COMPLETE_REVISION;

  HpobInstanceId = HygonPspGetHpobCddInstanceId (0, FabricTopologyGetFirstPhysCddIdOnSocket (0));
  Status = HygonPspGetHpobEntryInstance (HPOB_GROUP_MEM, HPOB_MEM_GENERAL_CONFIGURATION_INFO_TYPE, HpobInstanceId, FALSE, &HpobEntry);

  if ((Status != EFI_SUCCESS) || (HpobEntry == NULL)) {
    MemInitCompleteData->HygonMemoryFrequency = 0;
    MemInitCompleteData->DdrMaxRate = 0;
  } else {
    CopyMem (&HpobMemGenConfigHyEx, HpobEntry, sizeof (HPOB_MEM_GENERAL_CONFIGURATION_INFO_TYPE_STRUCT_HYEX));
    MemInitCompleteData->HygonMemoryFrequency = HpobMemGenConfigHyEx.MemClkFreq;
    MemInitCompleteData->DdrMaxRate = HpobMemGenConfigHyEx.DdrMaxRate;
    IDS_HDT_CONSOLE (MAIN_FLOW, "Ecc Enable = %d \n", HpobMemGenConfigHyEx.EccEnable[0]);
  }

  IDS_HDT_CONSOLE (MAIN_FLOW, "Memory Frequency = %d \n", MemInitCompleteData->HygonMemoryFrequency);
  IDS_HDT_CONSOLE (MAIN_FLOW, "DDR Max Rate = %d \n", MemInitCompleteData->DdrMaxRate);

  // ----------------------------------------------------------------
  //
  // Update with SPD data (If supported)
  //
  // ----------------------------------------------------------------
  // Initialize SPD data
  for (Socket = 0; Socket < HYGON_MEM_PPI_MAX_SOCKETS_SUPPORTED; Socket++) {
    for (Channel = 0; Channel < HYGON_MEM_PPI_MAX_CHANNELS_PER_SOCKET; Channel++) {
      for (Dimm = 0; Dimm < HYGON_MEM_PPI_MAX_DIMMS_PER_CHANNEL; Dimm++) {
        MemInitCompleteData->HygonDimmSpInfo.DimmSpdInfo[Socket][Channel][Dimm].SocketNumber  = Socket;
        MemInitCompleteData->HygonDimmSpInfo.DimmSpdInfo[Socket][Channel][Dimm].ChannelNumber = Channel;
        MemInitCompleteData->HygonDimmSpInfo.DimmSpdInfo[Socket][Channel][Dimm].DimmNumber    = Dimm;
        MemInitCompleteData->HygonDimmSpInfo.DimmSpdInfo[Socket][Channel][Dimm].DimmPresent   = FALSE;
        MemInitCompleteData->HygonDimmSpInfo.DimmSpdInfo[Socket][Channel][Dimm].MuxPresent    = 0;
        MemInitCompleteData->HygonDimmSpInfo.DimmSpdInfo[Socket][Channel][Dimm].Address = 0;
        MemInitCompleteData->HygonDimmSpInfo.DimmSpdInfo[Socket][Channel][Dimm].SerialNumber = 0;
        MemInitCompleteData->HygonDimmSpInfo.DimmSpdInfo[Socket][Channel][Dimm].SpdDataPtr   = 0;
      }
    }
  }

  // Update SPD data
  SocketNum = FabricTopologyGetNumberOfSocketPresent ();
  for (Socket = 0; Socket < SocketNum; Socket++) {
    IDS_HDT_CONSOLE (MAIN_FLOW, "Get Spd Data from HPOB for Socket %d\n", Socket);

    CddsPresentSocket = FabricTopologyGetCddsPresentOnSocket (Socket);
    for (CddLoop = 0; CddLoop < MAX_CDDS_PER_SOCKET; CddLoop++) {
      if (!IS_CDD_PRESENT (CddLoop, CddsPresentSocket)) {
        continue;
      }

      Instance = HygonPspGetHpobCddInstanceId (Socket, CddLoop);
      Status   = HygonPspGetHpobEntryInstance (HPOB_GROUP_MEM, HPOB_MEM_DIMM_SPD_DATA_TYPE, Instance, FALSE, &HpobEntry);
      if (Status == EFI_SUCCESS) {
        CopyMem (&HpobSpdHeaderHyEx, HpobEntry, sizeof (HPOB_MEM_DIMM_SPD_DATA_STRUCT_HYEX));
        for (Channel = 0; Channel < HpobSpdHeaderHyEx.MaxChannelsPerCdd; Channel++) {
          IDS_HDT_CONSOLE (MAIN_FLOW, "Get Spd Data from HPOB for Channel %d\n", Channel);
          for (Dimm = 0; Dimm < HpobSpdHeaderHyEx.MaxDimmsPerChannel; Dimm++) {
            IDS_HDT_CONSOLE (MAIN_FLOW, "Get Spd Data from HPOB for Dimm %d\n", Dimm);
            DimmIndex = Dimm + Channel * HPOB_MAX_DIMMS_PER_CHANNEL;
            if (HpobSpdHeaderHyEx.DimmSmbusInfo[DimmIndex].DimmPresent == TRUE) {
              ChannelIndex = (CddLoop * HYGON_MEM_PPI_MAX_CHANNELS_PER_CDD) + Channel;
              IDS_HDT_CONSOLE (MAIN_FLOW, "Get Spd Dimm Present: Socket %d ChannelIndex %d Dimm %d \n", Socket, ChannelIndex, Dimm);
              MemInitCompleteData->HygonDimmSpInfo.DimmSpdInfo[Socket][ChannelIndex][Dimm].DimmPresent   = HpobSpdHeaderHyEx.DimmSmbusInfo[DimmIndex].DimmPresent;
              MemInitCompleteData->HygonDimmSpInfo.DimmSpdInfo[Socket][ChannelIndex][Dimm].Address       = HpobSpdHeaderHyEx.DimmSmbusInfo[DimmIndex].Address;
              MemInitCompleteData->HygonDimmSpInfo.DimmSpdInfo[Socket][ChannelIndex][Dimm].MuxI2CAddress = HpobSpdHeaderHyEx.DimmSmbusInfo[DimmIndex].MuxI2CAddress;
              MemInitCompleteData->HygonDimmSpInfo.DimmSpdInfo[Socket][ChannelIndex][Dimm].MuxPresent    = HpobSpdHeaderHyEx.DimmSmbusInfo[DimmIndex].MuxPresent;
              MemInitCompleteData->HygonDimmSpInfo.DimmSpdInfo[Socket][ChannelIndex][Dimm].MuxChannel    = HpobSpdHeaderHyEx.DimmSmbusInfo[DimmIndex].MuxChannel;
              MemInitCompleteData->HygonDimmSpInfo.DimmSpdInfo[Socket][ChannelIndex][Dimm].SerialNumber  = HpobSpdHeaderHyEx.DimmSmbusInfo[DimmIndex].SerialNumber;

              IDS_HDT_CONSOLE (MAIN_FLOW, "DIMM Technology %d\n", HpobSpdHeaderHyEx.DimmSmbusInfo[DimmIndex].Technology);

              Status = (*PeiServices)->AllocatePool (PeiServices, SpdSize, &MemInitCompleteData->HygonDimmSpInfo.DimmSpdInfo[Socket][ChannelIndex][Dimm].SpdDataPtr);
              if (Status != EFI_SUCCESS) {
                IDS_HDT_CONSOLE (MAIN_FLOW, "Allocate buffer for Socket %d Channel %d Dimm %d SPD data fail (%r)\n", Socket, ChannelIndex, Dimm, Status);
                continue;
              }

              CopyMem (
                MemInitCompleteData->HygonDimmSpInfo.DimmSpdInfo[Socket][ChannelIndex][Dimm].SpdDataPtr,
                HpobSpdHeaderHyEx.DimmSmbusInfo[DimmIndex].Data,
                SpdSize
                );

              DumpBuffer (MemInitCompleteData->HygonDimmSpInfo.DimmSpdInfo[Socket][ChannelIndex][Dimm].SpdDataPtr, SpdSize);
            }
          }
        }
      }
    }
  }

  Status = (*PeiServices)->AllocatePool (PeiServices, sizeof (EFI_PEI_PPI_DESCRIPTOR), &MemInitPpiList);
  MemInitPpiList->Flags = (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST);
  MemInitPpiList->Guid  = &gHygonMemoryInitCompletePpiGuid;
  MemInitPpiList->Ppi   = MemInitCompleteData;
  ((HYGON_MEMORY_INIT_COMPLETE_PPI *)MemInitPpiList->Ppi)->GetSystemMemoryMap = GetSystemMemoryMapInternal;

  Status = (*PeiServices)->InstallPpi (PeiServices, MemInitPpiList);

  Status = HygonPspGetHpobEntryInstance (HPOB_GROUP_MEM, HPOB_MEM_NVDIMM_INFO_TYPE, 0, FALSE, &HpobEntry);
  if (Status == EFI_SUCCESS) {
    CopyMem (&NvdimmMemoryInfo, HpobEntry, sizeof (HPOB_MEM_NVDIMM_INFO_STRUCT_HYEX));
    if (BuildGuidDataHob (&gHygonNvdimmInfoHobGuid, &NvdimmMemoryInfo.NvdimmInfo[0][0], (sizeof (NvdimmMemoryInfo.NvdimmInfo) + sizeof (NvdimmMemoryInfo.NvdimmPresentInSystem))) == NULL) {
      Status = EFI_NOT_FOUND;
    }
  }

  return (Status);
}
