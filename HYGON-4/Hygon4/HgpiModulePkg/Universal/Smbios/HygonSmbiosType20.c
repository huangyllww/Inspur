/* $NoKeywords:$ */

/**
 * @file
 *
 * Generate SMBIOS type 20
 *
 * Contains code that generate SMBIOS type 20
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      HGPI
 * @e sub-project:  Universal
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
#include "HygonSmbios.h"
#include "HYGON.h"
#include "Porting.h"
#include "Filecode.h"
#include "Library/IdsLib.h"
#include "Library/BaseLib.h"
#include <MemDmi.h>
#include "Library/UefiBootServicesTableLib.h"
#include "Library/MemoryAllocationLib.h"
#include "Protocol/Smbios.h"
#include "Protocol/HygonSmbiosServicesProtocol.h"
#include "PiDxe.h"

#define FILECODE  UNIVERSAL_SMBIOS_HYGONSMBIOSTYPE20_FILECODE

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
/**
 *---------------------------------------------------------------------------------------
 *
 *  Generate SMBIOS type 20
 *
 *  Parameters:
 *    @param[in]       Smbios                       Pointer to EfiSmbiosProtocol
 *    @param[in]       MemDmiInfo                   Pointer to Memory DMI information
 *    @param[in]       Socket                       Socket number
 *    @param[in]       Channel                      Channel number
 *    @param[in]       Dimm                         Dimm number
 *    @param[in]       MemoryDeviceHandle           Handle of the device where the address range is mapped to
 *    @param[in]       MemoryArrayHandle            Handle of the array where the address range is mapped to
 *    @param[in]       StdHeader                    Handle to config for library and services
 *
 *    @retval          EFI_SUCCESS                  The Type 20 entry is added successfully.
 *
 *---------------------------------------------------------------------------------------
 */
#if 0                             // byo230922 +
EFI_STATUS
EFIAPI
HygonAddSmbiosType20 (
  IN       EFI_SMBIOS_PROTOCOL  *Smbios,
  IN       DMI_INFO             *MemDmiInfo,
  IN       UINT8                 Socket,
  IN       UINT8                 Channel,
  IN       UINT8                 Dimm,
  IN       EFI_SMBIOS_HANDLE     MemoryDeviceHandle,
  IN       EFI_SMBIOS_HANDLE     *MemoryArrayHandle,
  IN       HYGON_CONFIG_PARAMS    *StdHeader
  )
{
  UINTN                      StructureSize;
  UINTN                      TotalSize;
  EFI_STATUS                 Status;
  EFI_STATUS                 CalledStatus;
  HYGON_SMBIOS_TABLE_TYPE20  *SmbiosTableType20;
  UINT32                     MmioHoleSizeBelow4G;

  UINT64      MemStartAddr;
  EFI_STATUS  Status1 = EFI_SUCCESS;
  EFI_STATUS  Status2 = EFI_SUCCESS;

  Status = EFI_SUCCESS;
  MmioHoleSizeBelow4G = MemDmiInfo->T19[0].EndingAddr - MemDmiInfo->T19[0].StartingAddr + 1;
  // Generate DMI type 20 --- Start

  if (MemDmiInfo->T20[Socket][Channel][Dimm].EndingAddr != 0) {
    // Calculate size of DMI type 20
    StructureSize = sizeof (HYGON_SMBIOS_TABLE_TYPE20);
    TotalSize     = StructureSize + 2; // Additional null (00h), End of strings

    // Allocate zero pool
    SmbiosTableType20 = NULL;
    SmbiosTableType20 = AllocateZeroPool (TotalSize);
    if (SmbiosTableType20 == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }

    if (MemDmiInfo->T20[Socket][Channel][Dimm].StartingAddr != 0xFFFFFFFF) {
      MemStartAddr = MemDmiInfo->T20[Socket][Channel][Dimm].StartingAddr;
    } else {
      MemStartAddr = MemDmiInfo->T20[Socket][Channel][Dimm].ExtStartingAddr;
    }

    // assume that size of one dimm >= 4GB and total memory is less than (4TB - MmioHoleSizeBelow4G)
    if (MemStartAddr >= 0x400000) {
      // if dimm base address is above 4G, adjust starting and ending address by adding mmio size
      SmbiosTableType20->Hdr.Handle = HYGON_SMBIOS_HANDLE_PI_RESERVED;
      SmbiosTableType20->Hdr.Type   = HYGON_EFI_SMBIOS_TYPE_MEMORY_DEVICE_MAPPED_ADDRESS;
      SmbiosTableType20->Hdr.Length = (UINT8)StructureSize;
      SmbiosTableType20->MemoryDeviceHandle = MemoryDeviceHandle;
      SmbiosTableType20->MemoryArrayMappedAddressHandle = MemoryArrayHandle[1];      // memroy map address block above 4G
      SmbiosTableType20->PartitionRowPosition = MemDmiInfo->T20[Socket][Channel][Dimm].PartitionRowPosition;
      SmbiosTableType20->InterleavePosition   = MemDmiInfo->T20[Socket][Channel][Dimm].InterleavePosition;
      SmbiosTableType20->InterleavedDataDepth = MemDmiInfo->T20[Socket][Channel][Dimm].InterleavedDataDepth;

      if (MemDmiInfo->T20[Socket][Channel][Dimm].StartingAddr != 0xFFFFFFFF) {
        SmbiosTableType20->StartingAddress = MemDmiInfo->T20[Socket][Channel][Dimm].StartingAddr + MmioHoleSizeBelow4G;
        SmbiosTableType20->EndingAddress   = MemDmiInfo->T20[Socket][Channel][Dimm].EndingAddr + MmioHoleSizeBelow4G;
        SmbiosTableType20->ExtendedStartingAddress = MemDmiInfo->T20[Socket][Channel][Dimm].ExtStartingAddr;
        SmbiosTableType20->ExtendedEndingAddress   = MemDmiInfo->T20[Socket][Channel][Dimm].ExtEndingAddr;
      } else {
        SmbiosTableType20->StartingAddress = MemDmiInfo->T20[Socket][Channel][Dimm].StartingAddr;
        SmbiosTableType20->EndingAddress   = MemDmiInfo->T20[Socket][Channel][Dimm].EndingAddr;
        SmbiosTableType20->ExtendedStartingAddress = MemDmiInfo->T20[Socket][Channel][Dimm].ExtStartingAddr + MmioHoleSizeBelow4G;
        SmbiosTableType20->ExtendedEndingAddress   = MemDmiInfo->T20[Socket][Channel][Dimm].ExtEndingAddr + MmioHoleSizeBelow4G;
      }

      // Add DMI type 20
      CalledStatus = Smbios->Add (Smbios, NULL, &SmbiosTableType20->Hdr.Handle, (EFI_SMBIOS_TABLE_HEADER *)SmbiosTableType20);
      Status = (CalledStatus > Status) ? CalledStatus : Status;
      // Free pool
      FreePool (SmbiosTableType20);
      return Status;
    } else {
      // If memory address covers mmio hole below 4g, we split it into two ranges:
      // MemDmiInfo->T20[Socket][Channel][Dimm].StartingAddr ~ MemDmiInfo->T19[0].EndingAddr and
      // 4G ~ SmbiosTableType20->EndingAddress + MmioHoleSizeBelow4G
      SmbiosTableType20->Hdr.Handle = HYGON_SMBIOS_HANDLE_PI_RESERVED;
      SmbiosTableType20->Hdr.Type   = HYGON_EFI_SMBIOS_TYPE_MEMORY_DEVICE_MAPPED_ADDRESS;
      SmbiosTableType20->Hdr.Length = (UINT8)StructureSize;
      SmbiosTableType20->MemoryDeviceHandle = MemoryDeviceHandle;
      SmbiosTableType20->MemoryArrayMappedAddressHandle = MemoryArrayHandle[0];
      SmbiosTableType20->PartitionRowPosition = MemDmiInfo->T20[Socket][Channel][Dimm].PartitionRowPosition;
      SmbiosTableType20->InterleavePosition   = MemDmiInfo->T20[Socket][Channel][Dimm].InterleavePosition;
      SmbiosTableType20->InterleavedDataDepth = MemDmiInfo->T20[Socket][Channel][Dimm].InterleavedDataDepth;
      SmbiosTableType20->StartingAddress = MemDmiInfo->T20[Socket][Channel][Dimm].StartingAddr;
      SmbiosTableType20->EndingAddress   = MemDmiInfo->T19[0].EndingAddr;
      SmbiosTableType20->ExtendedStartingAddress = MemDmiInfo->T20[Socket][Channel][Dimm].ExtStartingAddr;
      SmbiosTableType20->ExtendedEndingAddress   = MemDmiInfo->T20[Socket][Channel][Dimm].ExtEndingAddr;

      // Add DMI type 20
      CalledStatus = Smbios->Add (Smbios, NULL, &SmbiosTableType20->Hdr.Handle, (EFI_SMBIOS_TABLE_HEADER *)SmbiosTableType20);
      Status1 = (CalledStatus > EFI_SUCCESS) ? CalledStatus : EFI_SUCCESS;

      SmbiosTableType20->Hdr.Handle = HYGON_SMBIOS_HANDLE_PI_RESERVED;
      SmbiosTableType20->Hdr.Type   = HYGON_EFI_SMBIOS_TYPE_MEMORY_DEVICE_MAPPED_ADDRESS;
      SmbiosTableType20->Hdr.Length = (UINT8)StructureSize;
      SmbiosTableType20->MemoryDeviceHandle = MemoryDeviceHandle;
      SmbiosTableType20->MemoryArrayMappedAddressHandle = MemoryArrayHandle[1];
      SmbiosTableType20->PartitionRowPosition = MemDmiInfo->T20[Socket][Channel][Dimm].PartitionRowPosition;
      SmbiosTableType20->InterleavePosition   = MemDmiInfo->T20[Socket][Channel][Dimm].InterleavePosition;
      SmbiosTableType20->InterleavedDataDepth = MemDmiInfo->T20[Socket][Channel][Dimm].InterleavedDataDepth;
      SmbiosTableType20->StartingAddress = MemDmiInfo->T19[1].StartingAddr;
      SmbiosTableType20->EndingAddress   = MemDmiInfo->T20[Socket][Channel][Dimm].EndingAddr + MmioHoleSizeBelow4G;
      SmbiosTableType20->ExtendedStartingAddress = MemDmiInfo->T20[Socket][Channel][Dimm].ExtStartingAddr;
      SmbiosTableType20->ExtendedEndingAddress   = MemDmiInfo->T20[Socket][Channel][Dimm].ExtEndingAddr;

      // Add DMI type 20
      CalledStatus = Smbios->Add (Smbios, NULL, &SmbiosTableType20->Hdr.Handle, (EFI_SMBIOS_TABLE_HEADER *)SmbiosTableType20);
      Status2 = (CalledStatus > EFI_SUCCESS) ? CalledStatus : EFI_SUCCESS;

      // Free pool
      FreePool (SmbiosTableType20);

      return (Status1 != EFI_SUCCESS) ? Status1 : ((Status2 != EFI_SUCCESS) ? Status2 : EFI_SUCCESS);
    }
  } else {
    return EFI_SUCCESS;
  }
}
#endif                            // byo230922 +

// byo231030 - >>
// byo230922 + >>
EFI_STATUS 
ByoAddSmbiosType20 (
  EFI_SMBIOS_PROTOCOL        *Smbios,
  HYGON_SMBIOS_TABLE_TYPE20  *T20,
  UINT16                     MemoryArrayMappedAddressHandle,
  UINT64                     StartAddr,
  UINT64                     EndAddr
  )
{
  UINT64  Start, End;

  if(StartAddr >= 0xFFFFFFFF){ 
    Start     = 0xFFFFFFFF;
    StartAddr = LShiftU64(StartAddr, 10);
  } else {
    Start     = StartAddr;
    StartAddr = 0;
  }
  if(EndAddr >= 0xFFFFFFFF){ 
    End = 0xFFFFFFFF;
    EndAddr = LShiftU64(EndAddr, 10);
  } else {
    End     = EndAddr;
    EndAddr = 0;
  }
  if(Start == 0xFFFFFFFF){
    End = 0xFFFFFFFF;
  }

  T20->Hdr.Handle                        = HYGON_SMBIOS_HANDLE_PI_RESERVED;
  T20->Hdr.Type                          = HYGON_EFI_SMBIOS_TYPE_MEMORY_DEVICE_MAPPED_ADDRESS;
  T20->Hdr.Length                        = sizeof(HYGON_SMBIOS_TABLE_TYPE20);
  T20->MemoryDeviceHandle                = 0;
  T20->MemoryArrayMappedAddressHandle    = MemoryArrayMappedAddressHandle;
  T20->PartitionRowPosition              = 0xFF;
  T20->InterleavePosition                = 0;
  T20->InterleavedDataDepth              = 0;
  T20->StartingAddress                   = (UINT32)Start;
  T20->EndingAddress                     = (UINT32)End;
  T20->ExtendedStartingAddress           = StartAddr;
  T20->ExtendedEndingAddress             = EndAddr;
  return Smbios->Add (Smbios, NULL, &T20->Hdr.Handle, (EFI_SMBIOS_TABLE_HEADER*)T20);
}


EFI_STATUS
EFIAPI
HygonAddSmbiosType20 (
  IN       EFI_SMBIOS_PROTOCOL  *Smbios,
  IN       DMI_INFO             *MemDmiInfo,
  IN       EFI_SMBIOS_HANDLE     *MemoryArrayHandle
  )
{
  HYGON_SMBIOS_TABLE_TYPE20         *SmbiosTableType20;
  UINTN                             i;

  SmbiosTableType20 = AllocateZeroPool(sizeof(HYGON_SMBIOS_TABLE_TYPE20) + 2);
  if (SmbiosTableType20 == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

//[0] (0,1BFFFF) (0,0)             0  - 70000000
//[1] (400000,423FFFF) (0,0)       4G - 10 9000 0000
//[2] (0,0) (0,0)
//
// 4G x 1K = 4T

  for(i=0;i<MAX_T19_REGION_SUPPORTED;i++){

    if(MemDmiInfo->T19[i].StartingAddr == 0 && MemDmiInfo->T19[i].EndingAddr == 0){
      continue;
    }
    
    IDS_HDT_CONSOLE (MAIN_FLOW, "[%d] (%x,%x) Ex(%lx,%lx)\n", i,
      MemDmiInfo->T19[i].StartingAddr, MemDmiInfo->T19[i].EndingAddr, 
      MemDmiInfo->T19[i].ExtStartingAddr, MemDmiInfo->T19[i].ExtEndingAddr);

    ByoAddSmbiosType20 (
      Smbios, 
      SmbiosTableType20, 
      MemoryArrayHandle[i], 
      MemDmiInfo->T19[i].StartingAddr,
      MemDmiInfo->T19[i].EndingAddr
      );    
  }
  
  FreePool (SmbiosTableType20);
  return EFI_SUCCESS;
}
// byo230922 + <<
// byo231030 - <<



/*----------------------------------------------------------------------------------------
 *                          L O C A L    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
