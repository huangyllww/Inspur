/* $NoKeywords:$ */

/**
 * @file
 *
 * HYGON CPU POST API, and related functions.
 *
 * Contains code that initialized the CPU after memory init.
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

  PspPei.c
  Init PSP device
  Intercept MemoryDiscovery protocol and
    a) Send mailbox to PSP to inform the DRAM information

Abstract:
--*/

#include "PiPei.h"
#include <Library/HobLib.h>
#include <Ppi/HygonPspRecoveryDetectPpi.h>
#include <Ppi/HygonPspCommonServicePpi.h>
#include <Ppi/EndOfPeiPhase.h>

#include "HGPI.h"
#include <Library/HygonBaseLib.h>
#include <Library/IdsLib.h>
#include <Library/HygonPspBaseLib.h>
#include <Library/HygonPspMboxLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/BaseLib.h>
#include <Library/HygonPspHpobLib.h>
#include <Library/HygonCapsuleLib.h>
#include <Ppi/HygonErrorLogPpi.h>
#include <Ppi/HygonErrorLogServicePpi.h>
#include <Library/FchBaseLib.h>
#include <Ppi/FabricTopologyServicesPpi.h>
#include <Filecode.h>

#define FILECODE  PSP_HYGONPSPPEI_HYGONPSPPEI_FILECODE

STATIC HYGON_PSP_COMMON_SERVICE_PPI  mPspCommonServicePpi = {
  GetFtpmControlAreaV2,
  SwitchPspMmioDecodeV2,
  CheckPspDevicePresentV2,
  CheckFtpmCapsV2,
  PSPEntryInfoV2,
  PspLibTimeOutV2,
  AcquirePspSmiRegMutexV2,
  ReleasePspSmiRegMutexV2
};

STATIC EFI_PEI_PPI_DESCRIPTOR  mPspCommonServicePpiList =
{
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gHygonPspCommonServicePpiGuid,
  &mPspCommonServicePpi
};

STATIC HYGON_PSP_RECOVERY_DETECT_PPI  mPspRecoveryDetectPpi = {
  0x01
};

STATIC EFI_PEI_PPI_DESCRIPTOR  mPspRecoveryDetectedPpiList =
{
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gHygonPspRecoveryDetectPpiGuid,
  &mPspRecoveryDetectPpi
};

extern  EFI_GUID  gHygonErrorLogServicePpiGuid;
extern  EFI_GUID  gHygonFabricTopologyServicesPpiGuid;

EFI_STATUS
EFIAPI
EndOfPeiSignalPpiCallback (
  IN  EFI_PEI_SERVICES                **PeiServices,
  IN  EFI_PEI_NOTIFY_DESCRIPTOR       *NotifyDesc,
  IN  VOID                            *InvokePpi
  );

EFI_PEI_NOTIFY_DESCRIPTOR  mEndOfPeiSignalPpiCallback = {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gEfiEndOfPeiSignalPpiGuid,
  EndOfPeiSignalPpiCallback
};

EFI_STATUS
EFIAPI
EndOfPeiSignalPpiCallback (
  IN  EFI_PEI_SERVICES                **PeiServices,
  IN  EFI_PEI_NOTIFY_DESCRIPTOR       *NotifyDesc,
  IN  VOID                            *InvokePpi
  )
{
  // This callback will only be called in S3 Path
  // Send Command to PSP to lock DF register
  IDS_HDT_CONSOLE_PSP_TRACE ("HygonPspPei.EndOfPeiSignalPpiCallback\n");

  PspMboxBiosLockDFReg ();
  return (EFI_SUCCESS);
}

/*++

Routine Description:

  PSP Driver Entry. Initialize PSP device and report DRAM info to PSP once found

Arguments:

Returns:

  EFI_STATUS

--*/
extern EFI_GUID  gHygonCapsuleStatusHobGuid;
EFI_STATUS
EFIAPI
HygonPspPeiDriverEntry (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{

  EFI_STATUS                              Status = EFI_SUCCESS;

  // PEI_HYGON_ERROR_LOG_SERVICE_PPI *ErrorLogService;
  // ERROR_LOG_DATA_STRUCT   ErrorLogData;
  HGPI_TESTPOINT (TpPspPeiEntry, NULL);
  IDS_HDT_CONSOLE_PSP_TRACE ("Psp.Drv.HygonPspPei Enter\n");
  if (HygonCapsuleGetStatus ()) {
    IDS_HDT_CONSOLE_PSP_TRACE ("Build CapsuleStatusHob\n");
    BuildGuidHob (&gHygonCapsuleStatusHobGuid, 1);
  }

  // Public PSP Common Service PPI V1 instance
  IDS_HDT_CONSOLE_PSP_TRACE ("\tInstall PSP common service PPI\n");
  Status = (**PeiServices).InstallPpi (PeiServices, &mPspCommonServicePpiList);
  if (EFI_ERROR (Status)) {
    IDS_HDT_CONSOLE_PSP_TRACE ("InstallPpi fail\n");
    ASSERT (FALSE);
  }

  // Check PSP Recovery required Flag, if set publish PPI
  if ((FchReadSleepType () != 3) && (HygonCapsuleGetStatus () == FALSE)) {
    if (CheckPspRecoveryFlag ()) {
      IDS_HDT_CONSOLE_PSP_TRACE ("Recovery Flag Detected\n");
      Status = (**PeiServices).InstallPpi (PeiServices, &mPspRecoveryDetectedPpiList);
      ASSERT (Status == EFI_SUCCESS);
    }
  }

  //
  // Always Initialize PSP BAR to support RdRand Instruction
  //
  IDS_HDT_CONSOLE_PSP_TRACE ("\tPsp BAR init\n");
  PspBarInitEarly ();
 
  // If PSP feature turn off, exit the driver
  if ((CheckPspDevicePresentV2 () == FALSE) ||
      (PcdGetBool (PcdHygonPspEnable) == FALSE)) {
    return EFI_SUCCESS;
  }

  //
  // Register end of PEI callback for S3
  //
  if (FchReadSleepType () == 0x03) {
    Status = (**PeiServices).NotifyPpi (PeiServices, &mEndOfPeiSignalPpiCallback);
    ASSERT (Status == EFI_SUCCESS);
    if (EFI_ERROR (Status)) {
      IDS_HDT_CONSOLE_PSP_TRACE ("Notify EndOfPei Ppi fail\n");
    }
  }

  IDS_HDT_CONSOLE_PSP_TRACE ("HygonPspPei Exit\n");
  HGPI_TESTPOINT (TpPspPeiExit, NULL);

  return (Status);
}
