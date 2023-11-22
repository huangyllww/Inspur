/* $NoKeywords:$ */

/**
 * @file
 *
 * HygonMemDmiConstruct.c
 *
 * Memory DMI table support.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: HGPI
 * @e sub-project: (Mem/Main)
 *
 **/
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
  * ***************************************************************************
  *
 */

/*
 *----------------------------------------------------------------------------
 *                                MODULES USED
 *
 *----------------------------------------------------------------------------
 */
#include <HygonCpmPei.h>
#include <Library/BaseLib.h>
#include <Library/MemoryAllocationLib.h>
#include "HGPI.h"
#include "Library/HygonBaseLib.h"
#include "Library/IdsLib.h"
#include <Library/PcdLib.h>
#include "Ids.h"
#include "Library/HygonHeapLib.h"
#include "MemDmi.h"
#include <HPOB.h>
#include "Library/HygonCalloutLib.h"
#include "Library/MemChanXLat.h"
#include "Library/MemSmbiosLib.h"
#include "Filecode.h"
#include <Guid/HygonMemoryInfoHob.h>
#include <Library/PeiServicesTablePointerLib.h>
#include <Library/HygonSocBaseLib.h>
#include <Porting.h>

CODE_GROUP (G2_PEI)
RDATA_GROUP (G2_PEI)

#define FILECODE  LIBRARY_MEMSMBIOSLIB_MEMSMBIOSV2LIB_FILECODE
extern EFI_GUID  gHygonNvdimmSpdInfoHobGuid;

/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------
 *                           TYPEDEFS AND STRUCTURES
 *
 *----------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------
 *                        PROTOTYPES OF LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */
VOID *
EFIAPI
BuildGuidDataHob (
  IN CONST EFI_GUID              *Guid,
  IN VOID                        *Data,
  IN UINTN                       DataLength
  );

/* -----------------------------------------------------------------------------*/

/**
 *  TranslateChannelInfo
 *
 *  Description:
 *     Translate the channel Id depending upon the channel translation table.
 *
 *  Parameters:
 *    @param[in]        RequestedChannelId - The requested channel Id
 *    @param[out]       *TranslatedChannelId - Pointer to the translated Id
 *    @param[in]        *XlatTable - Pointer to the memory channel translation table
 *
 */
VOID
STATIC
TranslateChannelInfo (
  IN       UINT8 RequestedChannelId,
  OUT   UINT8 *TranslatedChannelId,
  IN       HOST_TO_HPCB_CHANNEL_XLAT *XlatTable
  )
{
  UINT8  Index;

  *TranslatedChannelId = RequestedChannelId;
  if (XlatTable != NULL) {
    Index = 0;
    while (XlatTable->RequestedChannelId != 0xFF) {
      if (RequestedChannelId == XlatTable->RequestedChannelId) {
        *TranslatedChannelId = XlatTable->TranslatedChannelId;
        return;
      }

      XlatTable++;
    }
  }
}

/* -----------------------------------------------------------------------------*/

/**
 *  GetPhysicalDimmInfo
 *
 *  Description:
 *     Obtain the physical DIMM information from SPD data.
 *
 *  Parameters:
 *    @param[in, out]  T17           - Pointer to TYPE17_DMI_INFO
 *    @param[in]       *PhysicalDimm - Pointer to the physical DIMM info
 *    @param[in, out]  *NvdimmSpdInfo- Pointer to NVDIMM_SPD_INFO
 *    @param[in]       *StdHeader   - Pointer to HYGON_CONFIG_PARAMS
 *
 *    @retval          BOOLEAN
 *
 */
BOOLEAN
STATIC
GetPhysicalDimmInfo (
  IN OUT   TYPE17_DMI_INFO *T17,
  IN       UINT8           DramType,
  IN       UINT8           Socket,
  IN       UINT8           Channel,
  IN       UINT8           Dimm,
  IN       HPOB_MEM_DMI_PHYSICAL_DIMM  *PhysicalDimm,
  IN OUT   NVDIMM_SPD_INFO    **NvdimmSpdInfo,
  IN       HYGON_CONFIG_PARAMS  *StdHeader
  )
{
  UINT16                              i;
  UINT8                               DimmSpd[DDR5_SPD_SIZE];
  UINT16                              SpdSize;
  HGPI_READ_SPD_PARAMS                ReadSpd;
  HGPI_STATUS                         HgpiStatus;
  EFI_STATUS                          Status;  
  HYGON_CPM_TABLE_PPI                 *CpmTablePpiPtr;
  HYGON_CPM_DIMM_MAP_TABLE            *DimmMapPtr;
  UINT16                               DimmMapIndex;
  UINT32                               CpuModel;

  CONST EFI_PEI_SERVICES              **PeiServices = GetPeiServicesTablePointer();

  Status = (*PeiServices)->LocatePpi (
                             (CPM_PEI_SERVICES **)PeiServices,
                             &gHygonCpmTablePpiGuid,
                             0,
                             NULL,
                             (VOID **)&CpmTablePpiPtr
                             );
  if (EFI_ERROR (Status)) {
    IDS_HDT_CONSOLE (MAIN_FLOW, "T17 DIMM_MAP gHygonCpmTablePpiGuid not found\n");
    return FALSE;
  }
  
  IDS_HDT_CONSOLE (MAIN_FLOW, "T17 DIMM_MAP  Socket:0x%x  Channel: 0x%x  Dimm = %d\n", Socket, Channel, Dimm);
  T17->Handle = PhysicalDimm->Handle;
  T17->DeviceSet = 0;

  CpuModel = GetHygonSocModel();
  if (CpuModel == HYGON_EX_CPU) {
    DimmMapIndex = Socket * MAX_CHANNELS_PER_SOCKET_HYEX * MAX_DIMMS_PER_CHANNEL + Channel * MAX_DIMMS_PER_CHANNEL + Dimm;
    DimmMapPtr = CpmTablePpiPtr->CommonFunction.GetTablePtr2 (CpmTablePpiPtr, HYEX_CPM_SIGNATURE_DIMM_MAP);
  } else {
    DimmMapIndex = Socket * MAX_CHANNELS_PER_SOCKET_HYGX * MAX_DIMMS_PER_CHANNEL + Channel * MAX_DIMMS_PER_CHANNEL + Dimm;
    DimmMapPtr = CpmTablePpiPtr->CommonFunction.GetTablePtr2 (CpmTablePpiPtr, HYGX_CPM_SIGNATURE_DIMM_MAP);
  }
  

  if (DimmMapPtr != NULL) {
    IDS_HDT_CONSOLE (MAIN_FLOW, "Use CPM teble DIMM map\n");
    (*PeiServices)->CopyMem (T17->DeviceLocator, DimmMapPtr->T17DeviceLocator.T17DeviceLocatorInfo, sizeof (T17->DeviceLocator));
    /*locator should between the array*/
    if ((DimmMapPtr->T17DeviceLocator.SocketLocator > 0) && (DimmMapPtr->T17DeviceLocator.SocketLocator < (sizeof (T17->DeviceLocator) - 1))) {
      T17->DeviceLocator[DimmMapPtr->T17DeviceLocator.SocketLocator] = DimmMapPtr->DIMM[DimmMapIndex].Socket + 0x30;
    }
    if ((DimmMapPtr->T17DeviceLocator.CddLocator > 0) && (DimmMapPtr->T17DeviceLocator.CddLocator < (sizeof (T17->DeviceLocator) - 1))) {
      T17->DeviceLocator[DimmMapPtr->T17DeviceLocator.CddLocator] = DimmMapPtr->DIMM[DimmMapIndex].Cdd + 0x30;
    }
    if ((DimmMapPtr->T17DeviceLocator.UmcChlLocator > 0) && (DimmMapPtr->T17DeviceLocator.UmcChlLocator < (sizeof (T17->DeviceLocator) - 1))) {
      T17->DeviceLocator[DimmMapPtr->T17DeviceLocator.UmcChlLocator] = DimmMapPtr->DIMM[DimmMapIndex].UMCchannel + 0x30;
    }
    if ((DimmMapPtr->T17DeviceLocator.DimmLocator > 0) && (DimmMapPtr->T17DeviceLocator.DimmLocator < (sizeof (T17->DeviceLocator) - 1))) {
      T17->DeviceLocator[DimmMapPtr->T17DeviceLocator.DimmLocator] = DimmMapPtr->DIMM[DimmMapIndex].Dimm + 0x30;
    }
    if ((DimmMapPtr->T17DeviceLocator.ChannelNameLocator > 0) && (DimmMapPtr->T17DeviceLocator.ChannelNameLocator < (sizeof (T17->DeviceLocator) - 1))) {
      T17->DeviceLocator[DimmMapPtr->T17DeviceLocator.ChannelNameLocator] = DimmMapPtr->DIMM[DimmMapIndex].ChannelName;
    }

    (*PeiServices)->CopyMem (T17->BankLocator, DimmMapPtr->T17BankLocator.T17BankLocatorInfo, sizeof (T17->BankLocator));
    /*locator should between the array*/
    if ((DimmMapPtr->T17BankLocator.SocketLocator != 0) && (DimmMapPtr->T17BankLocator.SocketLocator < (sizeof (T17->BankLocator) - 1))) {
      T17->BankLocator[DimmMapPtr->T17BankLocator.SocketLocator] = DimmMapPtr->DIMM[DimmMapIndex].Socket + 0x30;
    }
    if ((DimmMapPtr->T17BankLocator.CddLocator != 0) && (DimmMapPtr->T17BankLocator.CddLocator < (sizeof (T17->BankLocator) - 1))) {
      T17->BankLocator[DimmMapPtr->T17BankLocator.CddLocator] = DimmMapPtr->DIMM[DimmMapIndex].Cdd + 0x30;
    }
    if ((DimmMapPtr->T17BankLocator.UmcChlLocator != 0) && (DimmMapPtr->T17BankLocator.UmcChlLocator < (sizeof (T17->BankLocator) - 1))) {
      T17->BankLocator[DimmMapPtr->T17BankLocator.UmcChlLocator] = DimmMapPtr->DIMM[DimmMapIndex].UMCchannel + 0x30;
    }
    if ((DimmMapPtr->T17BankLocator.DimmLocator != 0) && (DimmMapPtr->T17BankLocator.DimmLocator < (sizeof (T17->BankLocator) - 1))) {
      T17->BankLocator[DimmMapPtr->T17BankLocator.DimmLocator] = DimmMapPtr->DIMM[DimmMapIndex].Dimm + 0x30;
    }
    if ((DimmMapPtr->T17BankLocator.ChannelNameLocator != 0) && (DimmMapPtr->T17BankLocator.ChannelNameLocator < (sizeof (T17->BankLocator) - 1))) {
      T17->BankLocator[DimmMapPtr->T17BankLocator.ChannelNameLocator] = DimmMapPtr->DIMM[DimmMapIndex].ChannelName;
    }	
  } else {
  
    IDS_HDT_CONSOLE (MAIN_FLOW, "Use default dimm map\n");
    T17->DeviceLocator[0]  = 'P';
    T17->DeviceLocator[1]  = Socket + 0x30;
    T17->DeviceLocator[2]  = '_';
    T17->DeviceLocator[3]  = 'D';
    T17->DeviceLocator[4]  = 'I';
    T17->DeviceLocator[5]  = 'M';
    T17->DeviceLocator[6]  = 'M';
    T17->DeviceLocator[7]  = '_';
    T17->DeviceLocator[8]  = Channel + 0x41;
    T17->DeviceLocator[9]  = Dimm + 0x30;
    T17->DeviceLocator[10] = '\0';
  
    T17->BankLocator[0]  = 'P';
    T17->BankLocator[1]  = Socket + 0x30;
    T17->BankLocator[2]  = ' ';
    T17->BankLocator[3]  = 'C';
    T17->BankLocator[4]  = 'H';
    T17->BankLocator[5]  = 'A';
    T17->BankLocator[6]  = 'N';
    T17->BankLocator[7]  = 'N';
    T17->BankLocator[8]  = 'E';
    T17->BankLocator[9]  = 'L';
    T17->BankLocator[10] = ' ';
    T17->BankLocator[11] = Channel + 0x41;
    T17->BankLocator[12] = '\0';
  }

  IDS_HDT_CONSOLE (MAIN_FLOW, "T17 Handle = %x\n", T17->Handle);
  if (PhysicalDimm->DimmPresent) {
    // Set Memory Type
    if (DramType == DDR5_TECHNOLOGY) {
      T17->MemoryType = Ddr5MemType;
      SpdSize = DDR5_SPD_SIZE;
    } else {
      T17->MemoryType = Ddr4MemType;
      SpdSize = DDR4_SPD_SIZE;
    }

    // Get SPD data for this DIMM
    ReadSpd.SocketId     = PhysicalDimm->Socket;
    ReadSpd.MemChannelId = PhysicalDimm->Channel;
    ReadSpd.DimmId = PhysicalDimm->Dimm;
    ReadSpd.Buffer = &DimmSpd[0];
    LibHygonMemCopy (&ReadSpd.StdHeader, StdHeader, sizeof (HYGON_CONFIG_PARAMS), StdHeader);

    IDS_HDT_CONSOLE (MAIN_FLOW, "SPD Socket %d Channel %d Dimm %d: %08x\n", PhysicalDimm->Socket, PhysicalDimm->Channel, PhysicalDimm->Dimm, DimmSpd);
    HGPI_TESTPOINT (TpProcMemBeforeHgpiReadSpd, StdHeader);
    HgpiStatus = HgpiReadSpd (0, &ReadSpd);
    HGPI_TESTPOINT (TpProcMemAfterHgpiReadSpd, StdHeader);

    if(HGPI_SUCCESS != HgpiStatus) {
      return FALSE;
    }

    ASSERT (HGPI_SUCCESS == HgpiStatus);

    // Dump SPD data
    for (i = 0; i < SpdSize; i++) {
      if (0 == i % 16) {
        IDS_HDT_CONSOLE (MAIN_FLOW, "\n\t");
      }

      IDS_HDT_CONSOLE (MAIN_FLOW, "%02x ", DimmSpd[i]);
    }

    IDS_HDT_CONSOLE (MAIN_FLOW, "\n");

    if (DramType == DDR5_TECHNOLOGY) {
      ParseDdr5SpdToInitT17 (&DimmSpd[0], T17 , PhysicalDimm);
      ParseDdr5SpdToInitNvdimmSpdInfo (&DimmSpd[0], T17, NvdimmSpdInfo);
    } else {
      ParseDdr4SpdToInitT17 (&DimmSpd[0], T17);
      ParseDdr4SpdToInitNvdimmSpdInfo (&DimmSpd[0], T17, NvdimmSpdInfo);
    }
    
    return TRUE;
  } else {
    IDS_HDT_CONSOLE (MAIN_FLOW, "Dummy Type 17 Created for Socket %d, Channel %d, Dimm %d\n", PhysicalDimm->Socket, PhysicalDimm->Channel, PhysicalDimm->Dimm);
    T17->DataWidth  = 0xFFFF;
    T17->TotalWidth = 0xFFFF;
    T17->MemorySize = 0;
    T17->ExtSize    = 0;
    T17->TypeDetail.Unknown = 1;
    T17->FormFactor = UnknowFormFactor;
    T17->MemoryType = UnknownMemType;
    T17->Speed = 0;
    T17->ManufacturerIdCode = 0;

    for (i = 0; i < sizeof (T17->SerialNumber); i++) {
      T17->SerialNumber[i] = 0x0;
    }

    for (i = 0; i < sizeof (T17->PartNumber); i++) {
      T17->PartNumber[i] = 0x0;
    }

    T17->Attributes        = 0;
    T17->MinimumVoltage    = 0;
    T17->MaximumVoltage    = 0;
    T17->ConfigSpeed       = 0;
    T17->ConfiguredVoltage = 0;

    // add for smbios 3.2
    T17->MemoryTechnology = UnknownType;
    T17->MemoryOperatingModeCapability.RegField.Unknown = 1;
    T17->FirmwareVersion[0]   = '\0';
    T17->ModuleManufacturerId = 0;
    T17->ModuleProductId = 0;
    T17->MemorySubsystemControllerManufacturerId = 0;
    T17->MemorySubsystemControllerProductId = 0;
    T17->NonvolatileSize = 0;
    T17->VolatileSize    = 0;
    T17->CacheSize   = 0;
    T17->LogicalSize = 0;
    // add for smbios 3.3
    T17->ExtendedSpeed = 0;
    T17->ExtendedConfiguredMemorySpeed = 0;

    return FALSE;
  }
}

/* -----------------------------------------------------------------------------*/

/**
 *  GetLogicalDimmInfo
 *
 *  Description:
 *     Obtain the logical DIMM information for Type 20.
 *
 *  Parameters:
 *    @param[in]        T20 - Pointer to TYPE20_DMI_INFO
 *    @param[out]       *LogicalDimm - Pointer to the logical DIMM info
 *
 *    @retval         BOOLEAN
 *
 */
BOOLEAN
GetLogicalDimmInfo (
  IN OUT   TYPE20_DMI_INFO *T20,
  IN       HPOB_MEM_DMI_LOGICAL_DIMM *LogicalDimm
  )
{
  T20->PartitionRowPosition = 0xFF;
  T20->InterleavePosition   = 0;
  T20->InterleavedDataDepth = 0;

  if (1 == LogicalDimm->DimmPresent) {
    T20->MemoryDeviceHandle = LogicalDimm->MemoryDeviceHandle;
    if (1 == LogicalDimm->Interleaved) {
      T20->InterleavePosition   = 0xFF;
      T20->InterleavedDataDepth = 0xFF;
    }

    T20->StartingAddr    = LogicalDimm->StartingAddr;
    T20->EndingAddr      = LogicalDimm->EndingAddr;
    T20->ExtStartingAddr = LogicalDimm->UnifiedExtStartingAddr.ExtStartingAddr;
    T20->ExtEndingAddr   = LogicalDimm->UnifiedExtEndingAddr.ExtEndingAddr;
    IDS_HDT_CONSOLE (MAIN_FLOW, "\tT20->StartingAddr=%x\n", T20->StartingAddr);
    IDS_HDT_CONSOLE (MAIN_FLOW, "\tT20->EndingAddr=%x\n", T20->EndingAddr);
    IDS_HDT_CONSOLE (MAIN_FLOW, "\tT20->ExtStartingAddr=%x\n", T20->ExtStartingAddr);
    IDS_HDT_CONSOLE (MAIN_FLOW, "\tT20->ExtEndingAddr=%x\n", T20->ExtEndingAddr);
  }

  return TRUE;
}

/* -----------------------------------------------------------------------------*/

/**
 *  GetMemSmbios
 *
 *  Description:
 *     This is the common routine for getting DMI Type 16, Type 17, Type 19 and Type 20 related information.
 *
 *  Parameters:
 *    @param[in]        DramType - DRAM Type
 *    @param[in]        *HpobSmbiosInfo - Pointer to the HPOB SMBIOS data block
 *    @param[in]        *MemChanXLatTab - Pointer to the memory channel translation table
 *    @param[in]        *StdHeader - Pointer to HYGON_CONFIG_PARAMS
 *    @param[in]        *HygonMemoryInfoHob - Pointer to HYGON_MEMORY_INFO_HOB
 *
 *    @retval         HGPI_STATUS
 *
 */
HGPI_STATUS
GetMemSmbios (
  IN       UINT8                       DramType,
  IN       UINT8                       NumSockets,
  IN       VOID                        *HpobSmbiosInfo,
  IN       HOST_TO_HPCB_CHANNEL_XLAT   *HostToHpcbChanXLatTab,
  IN       HYGON_CONFIG_PARAMS           *StdHeader,
  IN       HYGON_MEMORY_INFO_HOB         *HygonMemoryInfoHob
  )
{
  UINT8                          Socket;
  UINT8                          Channel;
  UINT8                          BoardChannel;
  UINT8                          Dimm;
  UINT8                          MaxChannel;
  UINT8                          MaxDimm;
  UINT16                         DimmIndex;
  UINT8                          MaxPhysicalDimms;
  UINT8                          MaxLogicalDimms;
  UINT16                         MaxHandle;
  DMI_INFO                       *DmiBuffer;
  ALLOCATE_HEAP_PARAMS           AllocateHeapParams;
  HPOB_MEM_DMI_HEADER            *HpobMemDmiHeader;
  UINT32                         TotalMemSize;
  UINT32                         TotalHpobMemDmiSize;
  HPOB_MEM_DMI_PHYSICAL_DIMM     *PhysicalDimm;
  HPOB_MEM_DMI_LOGICAL_DIMM      *LogicalDimm;
  HPOB_MEM_DMI_PHYSICAL_DIMM     DummyPhysicalDimm;
  UINT8                          NumActiveDimms;
  BOOLEAN                        SocketScanned[MAX_SOCKETS_SUPPORTED];
  NVDIMM_SPD_INFO                *NvdimmSpdInfo;
  UINT8                          RegionCnt = 0;
  UINT32                         DescIndex;
  HYGON_MEMORY_RANGE_DESCRIPTOR  *Range;
  BOOLEAN                        MergeRegion;
  UINT32                         CpuModel;

  HOST_TO_HPCB_CHANNEL_XLAT  *HpcbToBoardChanXLatTab;

  // Get HpcbToBoard channel mapping table
  HpcbToBoardChanXLatTab = (HOST_TO_HPCB_CHANNEL_XLAT *)PcdGetPtr (PcdHpcbToBoardChanXLatTab);

  //
  // Allocate a buffer by heap function
  //
  AllocateHeapParams.BufferHandle = HYGON_DMI_INFO_BUFFER_HANDLE;
  AllocateHeapParams.RequestedBufferSize = sizeof (DMI_INFO);
  AllocateHeapParams.Persist = HEAP_SYSTEM_MEM;

  if (HeapAllocateBuffer (&AllocateHeapParams, StdHeader) != HGPI_SUCCESS) {
    ASSERT (FALSE);
    return HGPI_ERROR;
  }

  DmiBuffer = (DMI_INFO *)AllocateHeapParams.BufferPtr;

  IDS_HDT_CONSOLE (MAIN_FLOW, "\tDMI enabled\n");
  // Fill with 0x00
  LibHygonMemFill (DmiBuffer, 0x00, sizeof (DMI_INFO), StdHeader);

  ASSERT (HpobSmbiosInfo != NULL);
  HpobMemDmiHeader = (HPOB_MEM_DMI_HEADER *)HpobSmbiosInfo;
  MaxPhysicalDimms = HpobMemDmiHeader->MaxPhysicalDimms;
  MaxLogicalDimms  = HpobMemDmiHeader->MaxLogicalDimms;
  TotalMemSize     = 0;

  IDS_HDT_CONSOLE (MAIN_FLOW, "\tHPOB SMBIOS MaxPhysicalDimms = %d, MaxLogicalDimms = %d \n",
    HpobMemDmiHeader->MaxPhysicalDimms, MaxLogicalDimms  = HpobMemDmiHeader->MaxLogicalDimms);
  TotalHpobMemDmiSize = sizeof (HPOB_MEM_DMI_PHYSICAL_DIMM) * MaxPhysicalDimms + sizeof (HPOB_MEM_DMI_LOGICAL_DIMM) * MaxLogicalDimms;
  PhysicalDimm = (HPOB_MEM_DMI_PHYSICAL_DIMM *)AllocateZeroPool (TotalHpobMemDmiSize);
  ASSERT (PhysicalDimm != NULL);
  LibHygonMemCopy (PhysicalDimm, &HpobMemDmiHeader[1], TotalHpobMemDmiSize, StdHeader);

  // Type 16 construction
  DmiBuffer->T16.Location = 0x03;
  DmiBuffer->T16.Use = 0x03;
  DmiBuffer->T16.NumberOfMemoryDevices = MaxPhysicalDimms;
  DmiBuffer->T16.MemoryErrorCorrection = (HpobMemDmiHeader->EccCapable != 0) ? Dmi16MultiBitEcc : Dmi16NoneErrCorrection;

  // Allocate buffer to keep all NVDIMM module SPD data for APCI NFIT table
  AllocateHeapParams.BufferHandle = HYGON_MEM_MISC_HANDLES_END - 1;
  AllocateHeapParams.RequestedBufferSize = (sizeof (NVDIMM_SPD_INFO)) * MaxPhysicalDimms;
  AllocateHeapParams.Persist = HEAP_SYSTEM_MEM;
  if (HeapAllocateBuffer (&AllocateHeapParams, StdHeader) != HGPI_SUCCESS) {
    ASSERT (FALSE);
    return HGPI_ERROR;
  }

  NvdimmSpdInfo = (NVDIMM_SPD_INFO *)AllocateHeapParams.BufferPtr;

  IDS_HDT_CONSOLE (MAIN_FLOW, "\tSMBIOS NumSockets = %d\n", NumSockets);
  for (Socket = 0; Socket < NumSockets; Socket++) {
    SocketScanned[Socket] = FALSE;
  }

  // TYPE 17 entries are organized by physical DIMMs
  NumActiveDimms = 0;
  MaxChannel     = 0;
  MaxDimm   = 0;
  MaxHandle = 0;
  for (DimmIndex = 0; DimmIndex < MaxPhysicalDimms; DimmIndex++, PhysicalDimm++) {
    IDS_HDT_CONSOLE (MAIN_FLOW, "\tHPOB SMBIOS Socket %d , Channel %d Dimm %d Present %d Spdaddr %p configSpeed %x\n",
      PhysicalDimm->Socket,
      PhysicalDimm->Channel,
      PhysicalDimm->Dimm,
      PhysicalDimm->DimmPresent,
      PhysicalDimm->SpdAddr,
      PhysicalDimm->ConfigSpeed
    );
    Socket  = PhysicalDimm->Socket;
    SocketScanned[Socket] = TRUE;
    IDS_HDT_CONSOLE (MAIN_FLOW, "\tSocket %d Scanned\n", Socket);
	  // Translate Physical/Logic Channel to Hpcb Channel
    TranslateChannelInfo (PhysicalDimm->Channel, &BoardChannel, HostToHpcbChanXLatTab);
	  IDS_HDT_CONSOLE (MAIN_FLOW, "\tPhysicalDimm->Channel: %d -----> BoardChannel: %d\n", PhysicalDimm->Channel,BoardChannel);

    if (MaxChannel < PhysicalDimm->Channel) {
      MaxChannel = PhysicalDimm->Channel;
      IDS_HDT_CONSOLE (MAIN_FLOW, "\tMax Channel = %d\n", MaxChannel);
    }
    Dimm = PhysicalDimm->Dimm;
    if (MaxDimm < Dimm) {
      MaxDimm = Dimm;
      IDS_HDT_CONSOLE (MAIN_FLOW, "\tMax Dimm = %d\n", MaxDimm);
    }
    if (MaxHandle < PhysicalDimm->Handle) {
      MaxHandle = PhysicalDimm->Handle;
      IDS_HDT_CONSOLE (MAIN_FLOW, "\tMax Handle = %x\n", MaxHandle);
    }

    CpuModel = GetHygonSocModel();
    if (CpuModel == HYGON_GX_CPU) {
      if (PhysicalDimm->Channel > 4) {
	  	// convert channel ID to continuous channel ID
        PhysicalDimm->Channel -= 4 ;
      }
    }
    
    NvdimmSpdInfo->Socket      = Socket;
    NvdimmSpdInfo->Channel     = PhysicalDimm->Channel;
    NvdimmSpdInfo->Dimm        = Dimm;
    NvdimmSpdInfo->NvdimmIndex = DimmIndex;   // Fill the DimmIndex no matter it is NVDIMM or not
    // GetPhysicalDimmInfo will move NvdimmSpdInfo to the next available location before return

    IDS_HDT_CONSOLE (MAIN_FLOW, "\tType 17 Entry address = %08x\n", &DmiBuffer->T17[Socket][PhysicalDimm->Channel][Dimm]);
    if (GetPhysicalDimmInfo (
         &DmiBuffer->T17[Socket][PhysicalDimm->Channel][Dimm], 
         DramType, 
         Socket,
         PhysicalDimm->Channel, 
         Dimm, 
         PhysicalDimm, 
         &NvdimmSpdInfo, 
         StdHeader
         )) {
      NumActiveDimms ++;
    }

    TotalMemSize += (DmiBuffer->T17[Socket][PhysicalDimm->Channel][Dimm].MemorySize != 0x7FFF) ?
                    DmiBuffer->T17[Socket][PhysicalDimm->Channel][Dimm].MemorySize : DmiBuffer->T17[Socket][PhysicalDimm->Channel][Dimm].ExtSize;
  }

  // Create dummy T17 entries for unpopulated sockets
  for (Socket = 0; Socket < NumSockets; Socket++) {
    if (FALSE == SocketScanned[Socket]) {
      IDS_HDT_CONSOLE (MAIN_FLOW, "\tConstructing Socket %d\n", Socket);
      DummyPhysicalDimm.Socket = Socket;
      DummyPhysicalDimm.DimmPresent = FALSE;
      for (Channel = 0; Channel <= MaxChannel; Channel++) {
        IDS_HDT_CONSOLE (MAIN_FLOW, "\tConstructing Channel %d\n", Channel);
        DummyPhysicalDimm.Channel = Channel;
        for (Dimm = 0; Dimm <= MaxDimm; Dimm++) {
          IDS_HDT_CONSOLE (MAIN_FLOW, "\tConstructing Dimm %d\n", Dimm);
          DummyPhysicalDimm.Dimm   = Dimm;
          DummyPhysicalDimm.Handle = ++MaxHandle;
          IDS_HDT_CONSOLE (MAIN_FLOW, "\tDummy Handle = %x\n", DummyPhysicalDimm.Handle);
          IDS_HDT_CONSOLE (MAIN_FLOW, "\tType 17 Entry address = %08x\n", &DmiBuffer->T17[Socket][Channel][Dimm]);
          GetPhysicalDimmInfo (&DmiBuffer->T17[Socket][Channel][Dimm], DramType, Socket, Channel, Dimm, &DummyPhysicalDimm, &NvdimmSpdInfo, StdHeader);
        }
      }
    }
  }

  // Move NvdimmSpdInfo to the start location before create HOB
  NvdimmSpdInfo = (NVDIMM_SPD_INFO *)AllocateHeapParams.BufferPtr;

  // Create a HOB to publish the NvdimmSpdInfo, the function creating ACPI NFIT will use that
  if (BuildGuidDataHob (&gHygonNvdimmSpdInfoHobGuid, NvdimmSpdInfo, (sizeof (NVDIMM_SPD_INFO)) * MaxPhysicalDimms) == NULL) {
    return EFI_NOT_FOUND;
  }

  // Deallocate heap
  if (HeapDeallocateBuffer ((HYGON_MEM_MISC_HANDLES_END - 1), StdHeader) != HGPI_SUCCESS) {
    ASSERT (FALSE);
    return HGPI_ERROR;
  }

  // Pointer to DMI info of Logical DIMMs
  LogicalDimm = (HPOB_MEM_DMI_LOGICAL_DIMM *)PhysicalDimm;

  // TYPE 20 entries are organized by logical DIMMs
  IDS_HDT_CONSOLE (MAIN_FLOW, "Build T20 LogicalDimmInfo: \n");
  for (DimmIndex = 0; DimmIndex < MaxLogicalDimms; DimmIndex++, LogicalDimm++) {
    Socket = LogicalDimm->Socket;
    // Translate Logic Channel to Hpcb Channel
    //TranslateChannelInfo (LogicalDimm->Channel, &HpcbChannel, HostToHpcbChanXLatTab);
    // Translate Hpcb Channel to board Channel
    //TranslateChannelInfo (HpcbChannel, &BoardChannel, HpcbToBoardChanXLatTab);
    //LogicalDimm->Channel = BoardChannel;
    Dimm = LogicalDimm->Dimm;
    Channel = LogicalDimm->Channel;                                                    // byo230922 -
    GetLogicalDimmInfo (&DmiBuffer->T20[Socket][Channel][Dimm], LogicalDimm);          // byo230922 -
  }

  // TYPE 19
  if (HygonMemoryInfoHob != NULL) {
    IDS_HDT_CONSOLE (MAIN_FLOW, "BuildT19 HygonMemoryInfoHob->NumberOfDescriptor=%x\n", HygonMemoryInfoHob->NumberOfDescriptor);
    IDS_HDT_CONSOLE (MAIN_FLOW, "\tTotalMemSize=%x\n", TotalMemSize);
    for (DescIndex = 0; DescIndex < HygonMemoryInfoHob->NumberOfDescriptor; DescIndex++) {
      Range = (HYGON_MEMORY_RANGE_DESCRIPTOR *)&HygonMemoryInfoHob->Ranges[DescIndex];
      IDS_HDT_CONSOLE (MAIN_FLOW, "\tRange->Attribute=%x\n", Range->Attribute);
      IDS_HDT_CONSOLE (MAIN_FLOW, "\tRange->Base=%16lX\n", Range->Base);
      IDS_HDT_CONSOLE (MAIN_FLOW, "\tRange->Size=%16lX\n", Range->Size);

      MergeRegion = FALSE;
      if ((Range->Attribute == HYGON_MEMORY_ATTRIBUTE_AVAILABLE) || (Range->Attribute == HYGON_MEMORY_ATTRIBUTE_RESERVED)) {
        if (RegionCnt < MAX_T19_REGION_SUPPORTED) {
          if (RShiftU64 (Range->Base, 10) >= 0xFFFFFFFF || RShiftU64 (Range->Base + Range->Size, 10) - 1 >= 0xFFFFFFFF) {
            // In Byte
            DmiBuffer->T19[RegionCnt].StartingAddr    = 0xFFFFFFFFUL;
            DmiBuffer->T19[RegionCnt].EndingAddr      = 0xFFFFFFFFUL;
            DmiBuffer->T19[RegionCnt].ExtStartingAddr = Range->Base;
            DmiBuffer->T19[RegionCnt].ExtEndingAddr   = Range->Base + Range->Size - 1;
            DmiBuffer->T19[RegionCnt].PartitionWidth  = NumActiveDimms;
            if (RegionCnt != 0) {
              if (DmiBuffer->T19[RegionCnt-1].ExtEndingAddr + 1 == DmiBuffer->T19[RegionCnt].ExtStartingAddr) {
                IDS_HDT_CONSOLE (MAIN_FLOW, "\tCombine:\n");
                IDS_HDT_CONSOLE (MAIN_FLOW, "\tDmiBuffer->T19[%x].ExtStartingAddr=%16lX\n", RegionCnt-1, (UINT64)DmiBuffer->T19[RegionCnt-1].ExtStartingAddr);
                IDS_HDT_CONSOLE (MAIN_FLOW, "\tDmiBuffer->T19[%x].ExtEndingAddr=%16lX\n", RegionCnt-1, (UINT64)DmiBuffer->T19[RegionCnt-1].ExtEndingAddr);
                DmiBuffer->T19[RegionCnt-1].ExtEndingAddr = DmiBuffer->T19[RegionCnt].ExtEndingAddr;
                MergeRegion = TRUE;
              }
            }

            IDS_HDT_CONSOLE (MAIN_FLOW, "\tDmiBuffer->T19[%x].ExtStartingAddr=%16lX\n", RegionCnt, (UINT64)DmiBuffer->T19[RegionCnt].ExtStartingAddr);
            IDS_HDT_CONSOLE (MAIN_FLOW, "\tDmiBuffer->T19[%x].ExtEndingAddr=%16lX\n", RegionCnt, (UINT64)DmiBuffer->T19[RegionCnt].ExtEndingAddr);
          } else {
            // In KByte
            DmiBuffer->T19[RegionCnt].StartingAddr    = (UINT32)RShiftU64 (Range->Base, 10);
            DmiBuffer->T19[RegionCnt].EndingAddr      = (UINT32)RShiftU64 (Range->Base + Range->Size, 10) - 1;
            DmiBuffer->T19[RegionCnt].ExtStartingAddr = 0;
            DmiBuffer->T19[RegionCnt].ExtEndingAddr   = 0;
            DmiBuffer->T19[RegionCnt].PartitionWidth  = NumActiveDimms;
            if (RegionCnt != 0) {
              if (DmiBuffer->T19[RegionCnt-1].EndingAddr + 1 == DmiBuffer->T19[RegionCnt].StartingAddr) {
                IDS_HDT_CONSOLE (MAIN_FLOW, "\tCombine:\n");
                IDS_HDT_CONSOLE (MAIN_FLOW, "\tDmiBuffer->T19[%x].StartingAddr=%16lX\n", RegionCnt-1, (UINT64)DmiBuffer->T19[RegionCnt-1].StartingAddr);
                IDS_HDT_CONSOLE (MAIN_FLOW, "\tDmiBuffer->T19[%x].EndingAddr=%16lX\n", RegionCnt-1, (UINT64)DmiBuffer->T19[RegionCnt-1].EndingAddr);
                DmiBuffer->T19[RegionCnt-1].EndingAddr = DmiBuffer->T19[RegionCnt].EndingAddr;
                MergeRegion = TRUE;
              }
            }

            IDS_HDT_CONSOLE (MAIN_FLOW, "\tDmiBuffer->T19[%x].StartingAddr=%08X\n", RegionCnt, (UINT32)DmiBuffer->T19[RegionCnt].StartingAddr);
            IDS_HDT_CONSOLE (MAIN_FLOW, "\tDmiBuffer->T19[%x].EndingAddr=%08X\n", RegionCnt, (UINT32)DmiBuffer->T19[RegionCnt].EndingAddr);
          }

          if (MergeRegion == TRUE) {
            DmiBuffer->T19[RegionCnt].StartingAddr    = 0;
            DmiBuffer->T19[RegionCnt].EndingAddr      = 0;
            DmiBuffer->T19[RegionCnt].ExtStartingAddr = 0;
            DmiBuffer->T19[RegionCnt].ExtEndingAddr   = 0;
            DmiBuffer->T19[RegionCnt].PartitionWidth  = 0;
            continue;
          }

          RegionCnt++;
        } else {
          IDS_HDT_CONSOLE (MAIN_FLOW, "WARNING!That MAX_T19_REGION_SUPPORTED Size Not Enough!\n");
          break;
        }
      }
    }
  } else {
    IDS_HDT_CONSOLE (MAIN_FLOW, "WARNING!Data HygonMemoryInfoHob Not Ready\n");
  }

  return HGPI_SUCCESS;
}
