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

#include <Library/HobLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Protocol/AmdBoardIdProtocol.h>
#include "AmdBoardIdDxe.h"

extern  EFI_BOOT_SERVICES       *gBS;

/*********************************************************************************
 * Name: AmdBoardIdDxeInit
 *
 * Description
 *   Entry point of the AMD BOARD ID DXE driver
 *   Perform the configuration init, resource reservation, early post init
 *   and install all the supported protocol
 *
 * Input
 *   ImageHandle : EFI Image Handle for the DXE driver
 *   SystemTable : pointer to the EFI system table
 *
 * Output
 *   EFI_SUCCESS : Module initialized successfully
 *   EFI_ERROR   : Initialization failed (see error for more details)
 *
 *********************************************************************************/
EFI_STATUS
EFIAPI
AmdBoardIdDxeInit (
  IN       EFI_HANDLE         ImageHandle,
  IN       EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS          Status = EFI_SUCCESS;
  EFI_HANDLE          Handle;
  EFI_DXE_AMDBOARDID_PROTOCOL *mAmdBoardIdProtocol;
  AMD_BOARDID_INFO_HOB *AmdBoardIdInfoHob;
  AMD_EEPROM_ROOT_TABLE     *AmdEepromRootTable;

 //
  // Initialize the configuration structure and private data area
  //

  //Get Error Log buffer from HOB
  AmdBoardIdInfoHob = GetFirstGuidHob (&gAmdBoardIdHobGuid);
  AmdEepromRootTable = &(AmdBoardIdInfoHob->AmdEepromRootTable);

  DEBUG ((EFI_D_ERROR, "Table Signature: %g\n", &AmdEepromRootTable->TableSignature));
  DEBUG ((EFI_D_ERROR, "Platform Identification: Board ID 0x%02x, Revision ID 0x%02x\n", AmdEepromRootTable->PlatformId.BoardId, AmdEepromRootTable->PlatformId.RevisionId));
  DEBUG ((EFI_D_ERROR, "APCB Instance Number: %0x\n", AmdEepromRootTable->ApcbInstance));
  DEBUG ((EFI_D_ERROR, "Major Revision: %0x\n", AmdEepromRootTable->MajorRevision));
  DEBUG ((EFI_D_ERROR, "Minor Revision: %0x\n", AmdEepromRootTable->MinorRevision));
  DEBUG ((EFI_D_ERROR, "CRC32 Checksum: %0x\n", AmdEepromRootTable->Checksum));

  Handle = ImageHandle;

  Status = gBS->AllocatePool (EfiBootServicesData, sizeof (EFI_DXE_AMDBOARDID_PROTOCOL), &mAmdBoardIdProtocol);
  if (EFI_ERROR (Status)) {
    return Status;
  } else {
    //clear instances content
    gBS->SetMem (mAmdBoardIdProtocol, sizeof (EFI_DXE_AMDBOARDID_PROTOCOL), 0);
  }

  mAmdBoardIdProtocol->Revision = AMDBOARDID_PROTOCOL_REVISION;
  mAmdBoardIdProtocol->AmdEepromRootTable = AmdEepromRootTable;

  // Publish BoardID service Protocol
  Status = gBS->InstallProtocolInterface (
                &Handle,
                &gAmdBoardIdProtocolGuid,
                EFI_NATIVE_INTERFACE,
                mAmdBoardIdProtocol
                );

  return (Status);
}

