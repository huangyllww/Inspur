/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD CPU POST API, and related functions.
 *
 * Contains code that initialized the CPU after memory init.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  PSP
 * @e \$Revision: 309090 $   @e \$Date: 2014-12-10 02:28:05 +0800 (Wed, 10 Dec 2014) $
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

  PspPei.c
  Init PSP device
  Intercept MemoryDiscovery protocol and
    a) Send mailbox to PSP to inform the DRAM information

Abstract:
--*/

#include "PiPei.h"
#include <Library/HobLib.h>
#include <Ppi/AmdPspRecoveryDetectPpi.h>
#include <Ppi/AmdPspCommonServicePpi.h>
#include <Ppi/EndOfPeiPhase.h>

#include "AGESA.h"
#include <Library/AmdBaseLib.h>
#include <Library/IdsLib.h>
#include <Library/AmdPspBaseLibV2.h>
#include <Library/AmdPspMboxLibV2.h>
#include <Library/BaseMemoryLib.h>
#include <Library/BaseLib.h>
#include <Library/AmdPspApobLib.h>
#include <Library/AmdCapsuleLib.h>
#include <Ppi/AmdErrorLogPpi.h>
#include <Ppi/AmdErrorLogServicePpi.h>
#include <Library/FchBaseLib.h>
#include <Ppi/FabricTopologyServicesPpi.h>
#include <Filecode.h>

#define FILECODE PSP_AMDPSPPEIV2_AMDPSPPEIV2_FILECODE

STATIC AMD_PSP_COMMON_SERVICE_PPI mPspCommonServicePpi = {
  GetFtpmControlAreaV2,
  SwitchPspMmioDecodeV2,
  CheckPspDevicePresentV2,
  CheckFtpmCapsV2,
  PSPEntryInfoV2,
  PspLibTimeOutV2,
  AcquirePspSmiRegMutexV2,
  ReleasePspSmiRegMutexV2
};

STATIC EFI_PEI_PPI_DESCRIPTOR mPspCommonServicePpiList =
{
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gAmdPspCommonServicePpiGuid,
  &mPspCommonServicePpi
};

STATIC AMD_PSP_RECOVERY_DETECT_PPI mPspRecoveryDetectPpi = {
  0x01
};

STATIC EFI_PEI_PPI_DESCRIPTOR mPspRecoveryDetectedPpiList =
{
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gAmdPspRecoveryDetectPpiGuid,
  &mPspRecoveryDetectPpi
};

extern  EFI_GUID  gAmdErrorLogServicePpiGuid;
extern  EFI_GUID  gAmdFabricTopologyServicesPpiGuid;

EFI_STATUS
EFIAPI
EndOfPeiSignalPpiCallback (
  IN  EFI_PEI_SERVICES                **PeiServices,
  IN  EFI_PEI_NOTIFY_DESCRIPTOR       *NotifyDesc,
  IN  VOID                            *InvokePpi
  );


EFI_PEI_NOTIFY_DESCRIPTOR   mEndOfPeiSignalPpiCallback = {
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
  //This callback will only be called in S3 Path
  //Send Command to PSP to lock DF register
  IDS_HDT_CONSOLE_PSP_TRACE ("AmdPspPeiV2.EndOfPeiSignalPpiCallback\n");

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
extern EFI_GUID gAmdCapsuleStatusHobGuid;
EFI_STATUS
EFIAPI
AmdPspPeiV2DriverEntry (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS            Status = EFI_SUCCESS;
  APOB_TYPE_HEADER      *ApobEntries[APOB_ENTRY_INSTANCE_MAX];
  PEI_AMD_ERROR_LOG_PPI *AmdErrorLog;
  EVENT_LOG_STRUCT      *EventLogPtr;
  UINT16                i;
  AMD_PEI_FABRIC_TOPOLOGY_SERVICES_PPI  *FabricTopologyServices;
  UINT32                NumberOfSockets;
  UINT32                NumberOfDies;
  UINT32                socketId;
  UINT32                dieId;
  UINT32                InstanceId;

  //PEI_AMD_ERROR_LOG_SERVICE_PPI *ErrorLogService;
  //ERROR_LOG_DATA_STRUCT   ErrorLogData;
  AGESA_TESTPOINT (TpPspPeiV2Entry, NULL);
  IDS_HDT_CONSOLE_PSP_TRACE ("Psp.Drv.AmdPspPeiV2 Enter\n");
  if (AmdCapsuleGetStatus ()) {
    IDS_HDT_CONSOLE_PSP_TRACE ("Build CapsuleStatusHob\n");
   BuildGuidHob (&gAmdCapsuleStatusHobGuid, 1);
  }
  // Public PSP Common Service PPI V1 instance
  IDS_HDT_CONSOLE_PSP_TRACE ("\tInstall PSP common service PPI\n");
  Status = (**PeiServices).InstallPpi (PeiServices, &mPspCommonServicePpiList);
  if (EFI_ERROR (Status)) {
    IDS_HDT_CONSOLE_PSP_TRACE ("InstallPpi fail\n");
    ASSERT (FALSE);
  }

  //Check PSP Recovery required Flag, if set publish PPI
  if ((FchReadSleepType () != 3) && (AmdCapsuleGetStatus () == FALSE)) {
    if (CheckPspRecoveryFlagV2 ()) {
      IDS_HDT_CONSOLE_PSP_TRACE ("Recovery Flag Detected\n");
      Status = (**PeiServices).InstallPpi (PeiServices, &mPspRecoveryDetectedPpiList);
      ASSERT (Status == EFI_SUCCESS);
    }
  }
  //
  // Always Initialize PSP BAR to support RdRand Instruction
  //
  IDS_HDT_CONSOLE_PSP_TRACE ("\tPsp BAR init\n");
  PspBarInitEarlyV2 ();

  // Retrieve event log data from ApobEntries and insert to AmdErrorLog
  Status = (*PeiServices)->LocatePpi (
                                 PeiServices,
                                 &gAmdFabricTopologyServicesPpiGuid,
                                 0,
                                 NULL,
                                 &FabricTopologyServices
                                 );
  ASSERT (Status == EFI_SUCCESS);

  FabricTopologyServices->GetSystemInfo (&NumberOfSockets, NULL, NULL);

  IDS_HDT_CONSOLE (MAIN_FLOW, "APOB EventLog, start\n");
  for (socketId = 0; socketId < NumberOfSockets; socketId++) {
    FabricTopologyServices->GetProcessorInfo (socketId, &NumberOfDies, NULL, NULL, NULL);
    for (dieId = 0; dieId < NumberOfDies; dieId++) {
      InstanceId = (socketId << 8) | dieId;
      Status = AmdPspGetApobEntryInstance(APOB_GEN, APOB_GEN_EVENT_LOG_TYPE, InstanceId, FALSE, ApobEntries);

      if (Status == EFI_SUCCESS) {
        Status = (*PeiServices)->LocatePpi (PeiServices, &gAmdErrorLogPpiGuid, 0, NULL, &AmdErrorLog);
        if (Status == EFI_SUCCESS) {
          EventLogPtr = (EVENT_LOG_STRUCT *) *ApobEntries;
          for (i = 0; i < EventLogPtr->Count; i++) {
            IDS_HDT_CONSOLE (MAIN_FLOW, "class: %08x, info: %08x, dataA: %08x, dataB: %08x\n",
                             EventLogPtr->ApobEventStruct.AgesaEvent[i].EventClass,
                             EventLogPtr->ApobEventStruct.AgesaEvent[i].EventInfo,
                             EventLogPtr->ApobEventStruct.AgesaEvent[i].DataA,
                             EventLogPtr->ApobEventStruct.AgesaEvent[i].DataB);
            AmdErrorLog->AmdErrorLogPei (AmdErrorLog,
                                         EventLogPtr->ApobEventStruct.AgesaEvent[i].EventClass,
                                         EventLogPtr->ApobEventStruct.AgesaEvent[i].EventInfo,
                                         EventLogPtr->ApobEventStruct.AgesaEvent[i].DataA,
                                         EventLogPtr->ApobEventStruct.AgesaEvent[i].DataB,
                                         0,   // AmdErrorLog has DataParam3 & DataParam4, but even log data doesn't
                                         0);
          }
        }
      }
    }
  }
  IDS_HDT_CONSOLE (MAIN_FLOW, "APOB EventLog, end\n");

  //If PSP feature turn off, exit the driver
  if ((CheckPspDevicePresentV2 () == FALSE) ||
      (PcdGetBool (PcdAmdPspEnable) == FALSE)) {
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

  IDS_HDT_CONSOLE_PSP_TRACE ("AmdPspPeiV2 Exit\n");
  AGESA_TESTPOINT (TpPspPeiV2Exit, NULL);

  return (Status);
}

