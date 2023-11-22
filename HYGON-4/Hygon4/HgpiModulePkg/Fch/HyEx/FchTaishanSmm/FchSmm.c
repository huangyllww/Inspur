/* $NoKeywords:$ */

/**
 * @file
 *
 * FCH SMM Driver
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      HGPI
 * @e sub-project   FCH SMM Driver
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
#include "FchSmm.h"
#define FILECODE  UEFI_SMM_FCHSMM_FCHSMM_FILECODE

#include <Library/BaseMemoryLib.h>

// FCH_INIT_PROTOCOL                   gFchInitInSmm;
FCH_DATA_BLOCK                      gFchDataInSmm;
UINT8           *mFchPciIrqRoutingTable;
FCH_MEM_BACKUP  *mFchMemoryBackup = NULL;
extern UINT64   gS3MemorySaveTableAddress[];
extern UINTN    gS3MemorySaveTableSize;

EFI_STATUS
FchSmmRegistrationCenter (
  VOID
  )
{
  EFI_STATUS  Status;

  Status = FchSmmRegisterSwSmi ();
  Status = FchSmmRegisterPowerButtonSmi ();
  Status = FchSmmRegisterPeriodicTimerSmi ();
  // Status = FchSmmRegisterGpiSmi ();
  Status = FchSmmRegisterSxSmi ();
  Status = FchSmmRegisterIoTrapSmi ();
  return Status;
}

/*********************************************************************************
 * Name: FchSmmHyExEntryPoint
 *
 * Description
 *   Entry point of the HYGON FCH SMM driver
 *   copy FCH_INIT_PROTOCOL to SMM, and register Fch Smm callbacks
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
FchSmmHyExEntryPoint (
  IN       EFI_HANDLE         ImageHandle,
  IN       EFI_SYSTEM_TABLE   *SystemTable
  )

{
  FCH_INIT_PROTOCOL      *pFchCimxInitProtocol;
  FCH_SMM_INIT_PROTOCOL  *pFchSmmInitProtocol;
  EFI_HANDLE             FchSmmInitHandle;
  EFI_STATUS             Status;

  HGPI_TESTPOINT (TpFchSmmEntry, NULL);
  //
  // Initialize global variables
  //
  Status = gSmst->SmmAllocatePool (
                    EfiRuntimeServicesData,
                    0x100,
                    &mFchPciIrqRoutingTable
                    );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = gSmst->SmmAllocatePool (
                    EfiRuntimeServicesData,
                    gS3MemorySaveTableSize * sizeof (FCH_MEM_BACKUP),
                    &mFchMemoryBackup
                    );
  if (EFI_ERROR (Status)) {
    mFchMemoryBackup = NULL;
  } else {
    SetMem (mFchMemoryBackup, gS3MemorySaveTableSize * sizeof (FCH_MEM_BACKUP), 0x00);
  }

  Status = gSmst->SmmAllocatePool (
                    EfiRuntimeServicesData,
                    sizeof (FCH_SMM_INIT_PROTOCOL),
                    &pFchSmmInitProtocol
                    );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = gBS->LocateProtocol (
                  &gFchInitProtocolGuid,
                  NULL,
                  &pFchCimxInitProtocol
                  );
  if (!EFI_ERROR (Status)) {
    CopyMem (
      &gFchDataInSmm,
      pFchCimxInitProtocol->FchPolicy,
      sizeof (FCH_DATA_BLOCK)
      );
  } else {
    return EFI_ABORTED;
  }

  Status = FchSmmRegistrationCenter ();

  //
  // install SMM protocol
  //
  pFchSmmInitProtocol->Revision     = pFchCimxInitProtocol->Revision;
  pFchSmmInitProtocol->FchRev       = pFchCimxInitProtocol->FchRev;
  pFchSmmInitProtocol->FchSmmPolicy = &gFchDataInSmm;
  FchSmmInitHandle =  NULL;
  Status = gSmst->SmmInstallProtocolInterface (
                    &FchSmmInitHandle,
                    &gFchSmmInitProtocolGuid,
                    EFI_NATIVE_INTERFACE,
                    pFchSmmInitProtocol
                    );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  HGPI_TESTPOINT (TpFchSmmExit, NULL);
  return Status;
}
