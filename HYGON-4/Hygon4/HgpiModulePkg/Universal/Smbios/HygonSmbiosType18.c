/* $NoKeywords:$ */

/**
 * @file
 *
 * Generate SMBIOS type 18
 *
 * Contains code that generate SMBIOS type 18
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

#define FILECODE  UNIVERSAL_SMBIOS_HYGONSMBIOSTYPE18_FILECODE

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
HygonAddSmbiosType18 (
  IN       EFI_SMBIOS_PROTOCOL  *Smbios,
  OUT   EFI_SMBIOS_HANDLE    *MemoryErrorInfoHandle,
  IN       HYGON_CONFIG_PARAMS    *StdHeader
  )
{
  UINTN                      StructureSize;
  UINTN                      TotalSize;
  EFI_STATUS                 Status;
  EFI_STATUS                 CalledStatus;
  HYGON_SMBIOS_TABLE_TYPE18  *SmbiosTableType18;

  Status = EFI_SUCCESS;

  // Calculate size of DMI type 18

  StructureSize = sizeof (HYGON_SMBIOS_TABLE_TYPE18);
  TotalSize     = StructureSize + 2; // Additional null (00h), End of strings

  // Allocate zero pool
  SmbiosTableType18 = NULL;
  SmbiosTableType18 = AllocateZeroPool (TotalSize);
  if (SmbiosTableType18 == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  SmbiosTableType18->Hdr.Handle = HYGON_SMBIOS_HANDLE_PI_RESERVED;
  SmbiosTableType18->Hdr.Type   = HYGON_EFI_SMBIOS_TYPE_32BIT_MEMORY_ERROR_INFORMATION;
  SmbiosTableType18->Hdr.Length = (UINT8)StructureSize;

  SmbiosTableType18->ErrorType = HygonMemoryErrorOk;
  SmbiosTableType18->ErrorGranularity = HygonMemoryGranularityOtherUnknown;
  SmbiosTableType18->ErrorOperation   = HygonMemoryErrorOperationUnknown;
  SmbiosTableType18->VendorSyndrome   = 0;
  SmbiosTableType18->MemoryArrayErrorAddress = 0x80000000;
  SmbiosTableType18->DeviceErrorAddress = 0x80000000;
  SmbiosTableType18->ErrorResolution    = 0x80000000;

  // Add DMI type 18
  CalledStatus = Smbios->Add (Smbios, NULL, &SmbiosTableType18->Hdr.Handle, (EFI_SMBIOS_TABLE_HEADER *)SmbiosTableType18);
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
