/* $NoKeywords:$ */
/**
 * @file
 *
 * Generate SMBIOS type 20
 *
 * Contains code that generate SMBIOS type 20
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Universal
 * @e \$Revision$   @e \$Date$
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
#include "AmdSmbios.h"
#include "AMD.h"
#include "Porting.h"
#include "Filecode.h"
#include "Library/IdsLib.h"
#include "Library/BaseLib.h"
#include <MemDmi.h>
#include "Library/UefiBootServicesTableLib.h"
#include "Library/MemoryAllocationLib.h"
#include "Protocol/Smbios.h"
#include "Protocol/AmdSmbiosServicesProtocol.h"
#include "PiDxe.h"

#define FILECODE UNIVERSAL_SMBIOS_AMDSMBIOSTYPE20_FILECODE

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
EFI_STATUS
EFIAPI
AmdAddSmbiosType20 (
  IN       UINTN                DimmIndex,
  IN       EFI_SMBIOS_PROTOCOL  *Smbios,
  IN       DMI_INFO             *MemDmiInfo,
  IN       UINT8                 Socket,
  IN       UINT8                 Channel,
  IN       UINT8                 Dimm,
  IN       EFI_SMBIOS_HANDLE     MemoryDeviceHandle,
  IN       EFI_SMBIOS_HANDLE     *MemoryArrayHandle,
  IN       AMD_CONFIG_PARAMS    *StdHeader
  )
{
  UINT32                            MmioHoleSizeBelow4G;
  AMD_SMBIOS_TABLE_TYPE20           *SmbiosTableType20;
  UINT64                            MemSizeKB;
  UINT32                            MemKBEnd;
  UINT64                            MemBEnd;
  UINT64                            StartMem;
  UINT32                            StartMemKB;
  UINT64                            StartMemB;
  STATIC UINT64                     DimmTempAddr = 0;
  

  IDS_HDT_CONSOLE (MAIN_FLOW, "AmdAddSmbiosType20\n");

  if(MemDmiInfo->T17[Socket][Channel][Dimm].VolatileSize == 0){
    return EFI_SUCCESS;
  }

  SmbiosTableType20 = AllocateZeroPool(sizeof(AMD_SMBIOS_TABLE_TYPE20) + 2);
  if (SmbiosTableType20 == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  if(DimmIndex == 0){
    DimmTempAddr = 0;
    MmioHoleSizeBelow4G = (MemDmiInfo->T19[0].EndingAddr - MemDmiInfo->T19[0].StartingAddr + 1) << 10;
    MemSizeKB = RShiftU64(MemDmiInfo->T17[Socket][Channel][Dimm].VolatileSize + MmioHoleSizeBelow4G, 10);
    if(MemSizeKB >= 0xFFFFFFFF){
      MemKBEnd = 0xFFFFFFFF;
    } else {
      MemKBEnd = (UINT32)MemSizeKB - 1;
    }
    MemBEnd  = MemDmiInfo->T17[Socket][Channel][Dimm].VolatileSize + MmioHoleSizeBelow4G - 1;
    DimmTempAddr += MemBEnd + 1;

    SmbiosTableType20->Hdr.Handle                        = AMD_SMBIOS_HANDLE_PI_RESERVED;
    SmbiosTableType20->Hdr.Type                          = AMD_EFI_SMBIOS_TYPE_MEMORY_DEVICE_MAPPED_ADDRESS;
    SmbiosTableType20->Hdr.Length                        = sizeof(AMD_SMBIOS_TABLE_TYPE20);
    SmbiosTableType20->MemoryDeviceHandle                = MemoryDeviceHandle;
    SmbiosTableType20->MemoryArrayMappedAddressHandle    = MemoryArrayHandle[0];
    SmbiosTableType20->PartitionRowPosition              = MemDmiInfo->T20[Socket][Channel][Dimm].PartitionRowPosition;
    SmbiosTableType20->InterleavePosition                = MemDmiInfo->T20[Socket][Channel][Dimm].InterleavePosition;
    SmbiosTableType20->InterleavedDataDepth              = MemDmiInfo->T20[Socket][Channel][Dimm].InterleavedDataDepth;
    SmbiosTableType20->StartingAddress                   = 0;
    SmbiosTableType20->EndingAddress                     = MemDmiInfo->T19[0].EndingAddr;
    SmbiosTableType20->ExtendedStartingAddress           = 0;
    SmbiosTableType20->ExtendedEndingAddress             = LShiftU64(MemDmiInfo->T19[0].EndingAddr+1, 10) - 1;

    IDS_HDT_CONSOLE (MAIN_FLOW, "[%d] %X %X %lX %lX\n", DimmIndex,
                                                   SmbiosTableType20->StartingAddress, 
                                                   SmbiosTableType20->EndingAddress,
                                                   SmbiosTableType20->ExtendedStartingAddress,
                                                   SmbiosTableType20->ExtendedEndingAddress
                                                   );
    
    Smbios->Add(Smbios, NULL, &SmbiosTableType20->Hdr.Handle, (EFI_SMBIOS_TABLE_HEADER*)SmbiosTableType20);
    
    SmbiosTableType20->Hdr.Handle                        = AMD_SMBIOS_HANDLE_PI_RESERVED;
    SmbiosTableType20->Hdr.Type                          = AMD_EFI_SMBIOS_TYPE_MEMORY_DEVICE_MAPPED_ADDRESS;
    SmbiosTableType20->Hdr.Length                        = sizeof(AMD_SMBIOS_TABLE_TYPE20);
    SmbiosTableType20->MemoryDeviceHandle                = MemoryDeviceHandle;
    SmbiosTableType20->MemoryArrayMappedAddressHandle    = MemoryArrayHandle[1];
    SmbiosTableType20->PartitionRowPosition              = MemDmiInfo->T20[Socket][Channel][Dimm].PartitionRowPosition;
    SmbiosTableType20->InterleavePosition                = MemDmiInfo->T20[Socket][Channel][Dimm].InterleavePosition;
    SmbiosTableType20->InterleavedDataDepth              = MemDmiInfo->T20[Socket][Channel][Dimm].InterleavedDataDepth;
    SmbiosTableType20->StartingAddress                   = MemDmiInfo->T19[1].StartingAddr;
    SmbiosTableType20->EndingAddress                     = MemKBEnd;
    SmbiosTableType20->ExtendedStartingAddress           = LShiftU64(MemDmiInfo->T19[1].StartingAddr, 10);
    SmbiosTableType20->ExtendedEndingAddress             = MemBEnd;

    IDS_HDT_CONSOLE (MAIN_FLOW, "[%d] %X %X %lX %lX\n", DimmIndex,
                                                   SmbiosTableType20->StartingAddress, 
                                                   SmbiosTableType20->EndingAddress,
                                                   SmbiosTableType20->ExtendedStartingAddress,
                                                   SmbiosTableType20->ExtendedEndingAddress
                                                   );
    
    Smbios->Add (Smbios, NULL, &SmbiosTableType20->Hdr.Handle, (EFI_SMBIOS_TABLE_HEADER*)SmbiosTableType20);
    
  }else {

    StartMem = RShiftU64(DimmTempAddr, 10);
    if(StartMem >= 0xFFFFFFFF){ 
      StartMemKB = 0xFFFFFFFF;
    } else {
      StartMemKB = (UINT32)StartMem;
    }
    StartMemB  = DimmTempAddr;

    MemSizeKB = RShiftU64(MemDmiInfo->T17[Socket][Channel][Dimm].VolatileSize + DimmTempAddr, 10);
    if(MemSizeKB >= 0xFFFFFFFF){
      MemKBEnd = 0xFFFFFFFF;
    } else {
      MemKBEnd = (UINT32)MemSizeKB - 1;
    }
    MemBEnd  = MemDmiInfo->T17[Socket][Channel][Dimm].VolatileSize + DimmTempAddr - 1;
    DimmTempAddr += MemDmiInfo->T17[Socket][Channel][Dimm].VolatileSize;    

    SmbiosTableType20->Hdr.Handle                        = AMD_SMBIOS_HANDLE_PI_RESERVED;
    SmbiosTableType20->Hdr.Type                          = AMD_EFI_SMBIOS_TYPE_MEMORY_DEVICE_MAPPED_ADDRESS;
    SmbiosTableType20->Hdr.Length                        = sizeof(AMD_SMBIOS_TABLE_TYPE20);
    SmbiosTableType20->MemoryDeviceHandle                = MemoryDeviceHandle;
    SmbiosTableType20->MemoryArrayMappedAddressHandle    = MemoryArrayHandle[1];
    SmbiosTableType20->PartitionRowPosition              = MemDmiInfo->T20[Socket][Channel][Dimm].PartitionRowPosition;
    SmbiosTableType20->InterleavePosition                = MemDmiInfo->T20[Socket][Channel][Dimm].InterleavePosition;
    SmbiosTableType20->InterleavedDataDepth              = MemDmiInfo->T20[Socket][Channel][Dimm].InterleavedDataDepth;
    SmbiosTableType20->StartingAddress                   = StartMemKB;
    SmbiosTableType20->EndingAddress                     = MemKBEnd;
    SmbiosTableType20->ExtendedStartingAddress           = StartMemB;
    SmbiosTableType20->ExtendedEndingAddress             = MemBEnd;

    IDS_HDT_CONSOLE (MAIN_FLOW, "[%d] %X %X %lX %lX\n", DimmIndex,
                                                   SmbiosTableType20->StartingAddress, 
                                                   SmbiosTableType20->EndingAddress,
                                                   SmbiosTableType20->ExtendedStartingAddress,
                                                   SmbiosTableType20->ExtendedEndingAddress
                                                   );
    
    Smbios->Add (Smbios, NULL, &SmbiosTableType20->Hdr.Handle, (EFI_SMBIOS_TABLE_HEADER*)SmbiosTableType20);

  }

  FreePool (SmbiosTableType20);
  return EFI_SUCCESS;
}



/*----------------------------------------------------------------------------------------
 *                          L O C A L    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
