/* $NoKeywords:$ */
/**
 * @file
 *
 * Generate SMBIOS type 18
 *
 * Contains code that generate SMBIOS type 18
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

#define FILECODE UNIVERSAL_SMBIOS_AMDSMBIOSTYPE18_FILECODE

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
 *  Generate SMBIOS type 18
 *
 *  Parameters:
 *    @param[in]       Smbios                       Pointer to EfiSmbiosProtocol
 *    @param[out]      MemoryErrorInfoHandle        Handle of the device where error is detected
 *    @param[in]       StdHeader                    Handle to config for library and services
 *
 *    @retval          EFI_STATUS
 *
 *---------------------------------------------------------------------------------------
 */
EFI_STATUS
EFIAPI
AmdAddSmbiosType18 (
  IN        EFI_SMBIOS_PROTOCOL  *Smbios,
  IN 	  	DMI_INFO			 *MemDmiInfo,
  IN       UINT8                 Socket,
  IN       UINT8                 Channel,
  IN       UINT8                 Dimm,
  OUT   	EFI_SMBIOS_HANDLE    *MemoryErrorInfoHandle,
  IN        AMD_CONFIG_PARAMS    *StdHeader
  )
{
  UINTN                             StructureSize;
  UINTN                             TotalSize;
  EFI_STATUS                        Status;
  EFI_STATUS                        CalledStatus;
  AMD_SMBIOS_TABLE_TYPE18          *SmbiosTableType18;

  Status = EFI_SUCCESS;
  IDS_HDT_CONSOLE (MAIN_FLOW, "  AmdAddSmbiosType18.\n");

  // Calculate size of DMI type 18

  StructureSize = sizeof (AMD_SMBIOS_TABLE_TYPE18);
  TotalSize = StructureSize + 2; // Additional null (00h), End of strings

  // Allocate zero pool
  SmbiosTableType18 = NULL;
  SmbiosTableType18 = AllocateZeroPool (TotalSize);
  if (SmbiosTableType18 == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  SmbiosTableType18->Hdr.Handle                        = AMD_SMBIOS_HANDLE_PI_RESERVED;
  SmbiosTableType18->Hdr.Type                          = AMD_EFI_SMBIOS_TYPE_32BIT_MEMORY_ERROR_INFORMATION;
  SmbiosTableType18->Hdr.Length                        = (UINT8) StructureSize;
  
  IDS_HDT_CONSOLE (MAIN_FLOW, "ErrorType %x\n",MemDmiInfo->T18[Socket][Channel][Dimm].ErrorType);

  if(MemDmiInfo->T18[Socket][Channel][Dimm].ErrorType == 0){
  	SmbiosTableType18->ErrorType                         = AmdMemoryErrorOk;
  	SmbiosTableType18->ErrorGranularity                  = AmdMemoryGranularityOther;
  	SmbiosTableType18->ErrorOperation                    = AmdMemoryErrorOperationOther;
  	SmbiosTableType18->VendorSyndrome                    = 0x80000000L;
  	SmbiosTableType18->MemoryArrayErrorAddress           = 0;	//0x80000000;
  	SmbiosTableType18->DeviceErrorAddress                = 0;	//0x80000000;
  	SmbiosTableType18->ErrorResolution                   = 0;	//0x80000000;
  	}
  else{
  	SmbiosTableType18->ErrorType                         = MemDmiInfo->T18[Socket][Channel][Dimm].ErrorType;
  	SmbiosTableType18->ErrorGranularity                  = MemDmiInfo->T18[Socket][Channel][Dimm].ErrorGranularity;
  	SmbiosTableType18->ErrorOperation                    = MemDmiInfo->T18[Socket][Channel][Dimm].ErrorOperation;
  	SmbiosTableType18->VendorSyndrome                    = MemDmiInfo->T18[Socket][Channel][Dimm].VendorSyndrome;
  	SmbiosTableType18->MemoryArrayErrorAddress           = MemDmiInfo->T18[Socket][Channel][Dimm].MemoryArrayErrorAddress;
  	SmbiosTableType18->DeviceErrorAddress                = MemDmiInfo->T18[Socket][Channel][Dimm].DeviceErrorAddress;
  	SmbiosTableType18->ErrorResolution                   = MemDmiInfo->T18[Socket][Channel][Dimm].ErrorResolution;
  	}
  // Add DMI type 18
  CalledStatus = Smbios->Add (Smbios, NULL, &SmbiosTableType18->Hdr.Handle, (EFI_SMBIOS_TABLE_HEADER *) SmbiosTableType18);
  Status = (CalledStatus > Status) ? CalledStatus : Status;
  *MemoryErrorInfoHandle = SmbiosTableType18->Hdr.Handle;

  // Free pool
  FreePool (SmbiosTableType18);

  return Status;
}



/*----------------------------------------------------------------------------------------
 *                          L O C A L    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

